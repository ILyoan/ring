extern printd: fn(int) -> int;

let fac = fn(n: int) -> int {
	if n == 0 { 1 } else { n * fac(n-1) }
}

let main = fn() -> int {
	printd(fac(10))
}
