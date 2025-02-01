import { arch, cpus, release, type } from "node:os";

Deno.chdir("../../");
const serverBinPath = Deno.args[0];
const serverStartupSeconds = 1;

let process;
if (serverBinPath) {
  process = new Deno.Command(serverBinPath, { stdout: "null" })
    .spawn();
}

const createRequest = () =>
  new Request("http://localhost:8080", { method: "GET" });
const durationSeconds = 10;
const loadTests = [
  {
    name: `Single User`,
    createRequest,
    concurrentUsers: 1,
    durationSeconds,
  },
  {
    name: `30 users`,
    createRequest,
    concurrentUsers: 30,
    durationSeconds,
  },
  {
    name: `100 users`,
    createRequest,
    concurrentUsers: 100,
    durationSeconds,
  },
] as const;

type Report = Record<string, {
  requests: number;
  fastestMs: number;
  slowestMs: number;
}>;

console.log("# Host Information\n");

console.log(`- **Platform**: ${type()} ${release()} (${arch()})`);

// This is naïve, but it works on my machine:
const [cpu, ...others] = cpus();
console.log(`- **CPU**: ${1 + others.length}-core ${cpu.model}`);

const memoryInfo = Deno.systemMemoryInfo();
console.log(
  `- **Memory**: ${(memoryInfo.total / 1024 / 1024).toFixed(0)} MB (${
    (memoryInfo.free / 1024 / 1024).toFixed(0)
  } MB free)`,
);
console.log();

console.log(`# Benchmarks\n`);
if (serverBinPath) {
  console.log(`Using server \`${serverBinPath.split("/").at(-1)}\``);
}

for (const phase of loadTests) {
  await new Promise((resolve) =>
    setTimeout(resolve, serverStartupSeconds * 1000)
  );

  console.log(`\n# Phase: ${phase.name}\n`);

  console.log(
    `For this benchmark, send and wait on requests for ${phase.durationSeconds} seconds using ${phase.concurrentUsers} concurrent user${
      phase.concurrentUsers === 1 ? "" : "s"
    }.\n`,
  );

  const startTestMs = performance.now();
  const timeoutMs = phase.durationSeconds * 1000;

  let stop = false;
  const abortController = new AbortController();
  const endTest = setTimeout(() => {
    stop = true;
    abortController.abort();
  }, timeoutMs);

  const reports: Report[] = await Promise.all(
    Array(phase.concurrentUsers).fill(0).entries().map(async ([userId]) => {
      const report: Report = {};
      while (!stop) {
        const request = phase.createRequest();

        const now = performance.now();
        let status;
        try {
          const response = await fetch(request, {
            signal: abortController.signal,
          });
          status = response.statusText;
        } catch (error) {
          stop = true;
          clearTimeout(endTest);
          console.log(
            `- **Warning**: User ${userId}, request dropped ${String(error)}`,
          );
          return report;
        }
        const ms = performance.now() - now;

        const key = `${request.method} ${request.url} -> ${status}`;
        if (report[key]) {
          report[key].requests++;
          if (ms < report[key].fastestMs) {
            report[key].fastestMs = ms;
          }

          if (ms > report[key].slowestMs) {
            report[key].slowestMs = ms;
          }
        } else {
          report[key] = {
            requests: 1,
            fastestMs: ms,
            slowestMs: ms,
          };
        }
      }

      return report;
    }),
  );
  const ms = performance.now() - startTestMs;

  const report = reports.reduce(
    (report, current) => {
      Object.entries(current).forEach(([key, value]) => {
        if (report[key]) {
          report[key].requests += value.requests;
          if (value.fastestMs < report[key].fastestMs) {
            report[key].fastestMs = value.fastestMs;
          }

          if (value.slowestMs > report[key].slowestMs) {
            report[key].slowestMs = value.slowestMs;
          }
        } else {
          report[key] = { ...value };
        }
      });
      return report;
    },
    {} as Report,
  );

  const durationSeconds = ms / 1000;
  Object.entries(report).forEach(([key, value]) => {
    console.log(`\n## ${key}\n`);

    console.log("### Server Performance\n");
    console.log(
      `- Handled ${value.requests} requests in ${
        durationSeconds.toFixed(3)
      } seconds`,
    );
    console.log(
      `  - Requests per second: ${
        (value.requests / durationSeconds).toFixed(0)
      }`,
    );
    console.log(
      `  - Mean response time: ${(ms / value.requests).toFixed(3)} ms`,
    );

    console.log("\n### User Experience\n");
    console.log(
      `- For the ${phase.concurrentUsers} user${
        phase.concurrentUsers == 1 ? "" : "s"
      } in this phase:`,
    );
    console.log(
      `  - Fastest response time: ${value.fastestMs.toFixed(3)} ms`,
    );
    console.log(
      `  - Slowest response time: ${value.slowestMs.toFixed(3)} ms`,
    );
  });

  console.log();
}

if (process) {
  try {
    Deno.kill(process.pid);
  } catch (error) {
    console.log(
      `\n\n- **Critical**: The server probably crashed before the end of the run, OS responded with \`${
        String(error)
      }\` while cleaning up`,
    );
  }
}
