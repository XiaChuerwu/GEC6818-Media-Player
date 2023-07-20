#指定最终目标的名字，根据实际情况修改
TARGEAT:=main

#指定编译器名字，根据实际情况修改
CC:=arm-linux-gcc

#指定头文件的路径
INC:=-I ./inc
#如果头文件存在多个路径下，根据实际情况注意加
#INC+=-I 头文件路径

#指定库文件的路径及其名字
LIB:=-L /opt/arm_libs/lib -l jpeg
#如果库文件有多个路径 同样


#指定.c文件的路径
SRCPATH:=./src

#根据路径得到 .o 文件列表
SRCNAME:=$(wildcard $(SRCPATH)/*.c)
OBJNAME:=$(patsubst %.c,%.o,$(SRCNAME))

# $(TARGEAT):main.o tree.o
# 	$(CC) main.o tree.o libqueue.a -o $(TARGEAT)

# $(TARGEAT):main.o tree.o
# 	$(CC) main.o tree.o $(LIB) -o $(TARGEAT)

#需要库的
$(TARGEAT):$(OBJNAME)
	$(CC) $(OBJNAME) $(LIB) -o $(TARGEAT)
	cp $(TARGEAT) /home/sen/tftpboot

# $(TARGEAT):$(OBJNAME)
# 	$(CC) $(OBJNAME) -o $(TARGEAT)
# 	cp $(TARGEAT) /home/sen/tftpboot

#测试
#	echo $(SRCNAME)
#	echo $(OBJNAME)


%.o : %.c
	$(CC) $(INC) -c $^ -o $@

# %.o : %.c
# 	$(CC) -c $^ -o -I 指定头文件路径或者 $(INC)  $@

clear:
	rm $(OBJNAME) $(TARGEAT)