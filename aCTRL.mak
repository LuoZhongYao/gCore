gm 	:= tools/usr/bin/gm
%.c %.h : %.ls
	$(gm) $^ -o ${^:%.ls=%}
