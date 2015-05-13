extern printd: fn(int) -> int;

let ret_nil_1: fn(int) -> nil = fn(a: int) {
	printd(a);
}

let ret_nil_2: fn() = fn() -> nil {
	printd(2);
}

let main: fn() = fn() {
	ret_nil_1(1);
	ret_nil_2();
}
