extern printd: fn(int) -> int;

let foo: fn() -> int = fn() -> int {
	let a : int = 1 + 2;
	let b : int = a + 3;
	let c : int = a + b;
	c
}

let main: fn() -> int = fn() -> nil {
	printd(foo() + foo());
}
