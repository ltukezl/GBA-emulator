int magicFunc(int a){
	a *= a;
	return a - 5;
}

int notmain(){
	int amount = 5;
	int list[5] = { 5, 6, 3, 2, 1 };
	int list2[5];
	int i = 0;
	for (i; i < amount; i++){
		list2[i] = magicFunc(list[i]);
	}
	return 0;
}