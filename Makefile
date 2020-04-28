CFLAG = -DDEBUG -Wall -std=c99

main: main.o scheduler.o process.o
	gcc $(CFLAG) main.o scheduler.o process.o -o main
main.o: main.c Makefile
	gcc $(CFLAG) main.c -c
scheduler.o: scheduler.c scheduler.h Makefile
	gcc $(CFLAG) scheduler.c -c
process.o: process.c process.h Makefile
	gcc $(CFLAG) process.c -c
clean:
	rm -rf *o
run:
	sudo ./main


input_file?=./sample.txt
test_files=OS_PJ1_Test
demo_files=TIME_MEASUREMENT FIFO_1 PSJF_2 RR_3 SJF_4
output_dir=output

test:
	@echo
	dmesg --clear
	./main < $(test_files)/$(input_file).txt > $(output_dir)/$(input_file)_output.txt
	dmesg | grep project1 > $(output_dir)/$(input_file)_dmesg.txt
	@echo

generate_test:
	for i in `ls $(test_files)`; do make test input_file=$$(basename $$i .txt); done;

demo_test:
	@echo
	dmesg --clear
	./main < $(test_files)/$(input_file).txt
	dmesg | grep project1
	@echo

generate_demo:
	for i in $(demo_files); do make demo_test input_file=$$i; done;
