extern printd: fn(int) -> int;

let main = fn() {
	let a = 1;
	let b = 2;
	printd(a + b);
}
