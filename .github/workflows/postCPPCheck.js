const fs = require('fs');
const github = require('@actions/github');

function parse(input) {
    const result = []
    input = input.split('\n').filter(line => [".cpp", ".h"].some(ext => line.includes(ext)));
    for(let line of input) {
        const match = line.match(/\/([^\/]+(?:cpp|h)):(\d+):(\d+):\s+(?:style:)?(.+)/) //const [file, line, column, message]
        if(match) {
            result.push({
                file: match[1],
                line: parseInt(match[2]),
                column: parseInt(match[3]),
                message: match[4]
            })
        }
    }
    return result
}
function format(result) {
    const groups = {}

    for(const item of result) {
        groups[item.file] ||= []
        groups[item.file].push(item)
    }

    let outputStr = ''

    for(const [file, items] of Object.entries(groups)) {
        outputStr += `### ${file}\n`
        outputStr += items.map(item => {
            return `\`${item.line}:${item.column}\` ${item.message}`
        }).join('\n')
        outputStr += '\n\n'
    }

    return outputStr
}


async function main() {
    const token = process.env.GITHUB_TOKEN;
    if (!token) throw new Error("Missing GITHUB_TOKEN env variable");

    const octokit = github.getOctokit(token);
    const context = github.context;

    const cppOutput = fs.readFileSync(process.env.OUTPUT, 'utf8');
    const result = format(parse(cppOutput));

    if (!context.payload.pull_request) {
        console.log("Not a pull request — skipping comment");
        return;
    }

    await octokit.rest.issues.createComment({
        issue_number: context.issue.number,
        owner: context.repo.owner,
        repo: context.repo.repo,
        body: result.slice(0, 6000)
    });
}

main().catch(err => {
    console.error(err);
    process.exit(1);
});