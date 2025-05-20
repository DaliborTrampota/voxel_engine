const fs = require('fs');

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

const file = process.env.OUTPUT;
if (!file) throw new Error("Missing OUTPUT env variable");

const cppOutput = fs.readFileSync(file, 'utf8');
console.log(format(parse(cppOutput)))