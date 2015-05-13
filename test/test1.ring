extern printd: fn(int) -> int;

let main: fn() -> int = fn() -> int {
	let a:int = 1;
	let b:int = 2;
	b = b + a;
	printd(a);
	printd(b);
	a
}
