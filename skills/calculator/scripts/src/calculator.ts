import Big, { BigSource } from 'big.js';

export class Calculator {
    private x: Big;

    constructor(initial: BigSource = new Big(0)) {
    this.x = new Big(initial);
    }

    add(v: string) { this.x = this.x.plus(v); return this; }
    sub(v: string) { this.x = this.x.minus(v); return this; }
    mul(v: string) { this.x = this.x.times(v); return this; }
    div(v: string) { this.x = this.x.div(v); return this; }

    toString() { return this.x.toString(); }

    static evaluate(expression: string): string {
        // Remove whitespace
        expression = expression.replace(/\s+/g, '');

        // Tokenize: numbers (including decimals) or operators/parens
        const tokens = expression.match(/(\d+(\.\d+)?)|[\+\-\*\/\(\)]/g);
        if (!tokens) return "0";

        const outputQueue: string[] = [];
        const operatorStack: string[] = [];
        const precedence: { [key: string]: number } = {
            '+': 1,
            '-': 1,
            '*': 2,
            '/': 2
        };

        for (const token of tokens) {
            if (!isNaN(parseFloat(token))) {
                outputQueue.push(token);
            } else if (token in precedence) {
                while (
                    operatorStack.length > 0 &&
                    operatorStack[operatorStack.length - 1] !== '(' &&
                    precedence[operatorStack[operatorStack.length - 1]] >= precedence[token]
                ) {
                    outputQueue.push(operatorStack.pop()!);
                }
                operatorStack.push(token);
            } else if (token === '(') {
                operatorStack.push(token);
            } else if (token === ')') {
                while (operatorStack.length > 0 && operatorStack[operatorStack.length - 1] !== '(') {
                    outputQueue.push(operatorStack.pop()!);
                }
                if (operatorStack.length === 0) throw new Error("Mismatched parentheses");
                operatorStack.pop();
            }
        }

        while (operatorStack.length > 0) {
            const op = operatorStack.pop()!;
            if (op === '(') throw new Error("Mismatched parentheses");
            outputQueue.push(op);
        }

        const evalStack: Big[] = [];
        for (const token of outputQueue) {
            if (!isNaN(parseFloat(token))) {
                evalStack.push(new Big(token));
            } else {
                const b = evalStack.pop();
                const a = evalStack.pop();
                if (!a || !b) throw new Error("Invalid expression");

                switch (token) {
                    case '+': evalStack.push(a.plus(b)); break;
                    case '-': evalStack.push(a.minus(b)); break;
                    case '*': evalStack.push(a.times(b)); break;
                    case '/': evalStack.push(a.div(b)); break;
                }
            }
        }

        if (evalStack.length !== 1) throw new Error("Invalid expression");
        return evalStack[0].toString();
    }
}
