Deno.chdir("../../");
const serverBinPath = Deno.args[0];
const serverStartupSeconds = 0.5;

const loadTests = [
  {
    createRequest: () =>
      new Request("http://localhost:8080", { method: "GET" }),
    phases: [
      {
        name: `Single User`,
        concurrentUsers: 1,
        durationSeconds: 10,
        timeoutMs: 100,
      },
      {
        name: `30 users`,
        concurrentUsers: 30,
        durationSeconds: 10,
        timeoutMs: 100,
      },
      {
        name: `100 users`,
        concurrentUsers: 100,
        durationSeconds: 10,
        timeoutMs: 100,
      },
    ],
  },
] as const;

type Report = Record<string, {
  requests: number;
  fastestMs: number;
  slowestMs: number;
}>;

for (const test of loadTests) {
  console.log(`# Load Test Report\n`);
  console.log(`Using server \`${serverBinPath.split("/").at(-1)}\``);

  for (const phase of test.phases) {
    const process = new Deno.Command(serverBinPath, { stdout: "null" }).spawn();
    await new Promise((resolve) =>
      setTimeout(resolve, serverStartupSeconds * 1000)
    );

    console.log(`\n# Phase: ${phase.name}\n`);

    console.log(
      `For this benchmark, send and wait on requests for ${phase.durationSeconds} seconds using ${phase.concurrentUsers} concurrent user${
        phase.concurrentUsers === 1 ? "" : "s"
      }.\n`,
    );

    console.log(
      `Cancel this benchmark if a request takes longer than ${phase.timeoutMs} milliseconds.\n`,
    );

    let stop = false;
    const now = performance.now();
    const endTest = setTimeout(() => stop = true, phase.durationSeconds * 1000);
    const reports: Report[] = await Promise.all(
      Array(phase.concurrentUsers).fill(0).map(async () => {
        const report: Report = {};
        while (!stop) {
          const request = test.createRequest();

          const now = performance.now();
          const response = await fetch(request);
          const ms = performance.now() - now;

          const key =
            `${request.method} ${request.url} -> ${response.statusText}`;
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

          if (ms > phase.timeoutMs) {
            stop = true;
            clearTimeout(endTest);
          }
        }

        return report;
      }),
    );
    const ms = performance.now() - now;

    Deno.kill(process.pid);

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
      console.log(
        `- ${value.requests} requests in ${
          durationSeconds.toFixed(3)
        } seconds (${
          (value.requests / durationSeconds).toFixed(0)
        } requests/s)`,
      );

      console.log(
        `- Fastest in ${value.fastestMs.toFixed(3)} ms`,
      );
      console.log(
        `- Slowest in ${value.slowestMs.toFixed(3)} ms`,
      );
      console.log();
    });
  }
}
