// These dependencies are insane, this is just a proof of concept for ozone web components
import remarkGfm from 'remark-gfm';
import remarkParse from 'remark-parse';
import remarkRehype from 'remark-rehype';
import rehypeStringify from 'rehype-stringify';
import { unified } from 'unified';

export class OzoneMarkdown extends HTMLElement {
  constructor() {
    super();
  }
  async connectedCallback() {
    console.log(this.innerText);
    const html = await unified()
      .use(remarkParse)
      .use(remarkGfm)
      .use(remarkRehype)
      .use(rehypeStringify)
      .process(this.innerText);

    this.innerHTML = String(html);
  }
};
