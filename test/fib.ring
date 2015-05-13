extern printd: fn(int) -> int;

let n = fn() -> int { 40 }

let fib = fn(n: int) -> int {
	if n == 0 { 1 }
	else if n == 1 { 1 }
	else { fib(n-1) + fib(n-2) }
}

let main = fn() -> int {
	printd(fib(n()))
}
