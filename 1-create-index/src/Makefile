#
NAME1=process
NAME2=archv

CC = g++
CFLAGS = -c -O
LDFLAGS = -O 
LIBS=-L/usr/local/lib/
LIBRARIES=-lopencv_core -lopencv_nonfree -lopencv_imgproc -lopencv_highgui -lopencv_features2d -lopencv_flann -lopencv_contrib -lopencv_ml -lopencv_objdetect -lopencv_video -lopencv_videostab -lopencv_calib3d -lopencv_ocl -lopencv_photo -lopencv_stitching


all: 
	$(CC) $(NAME1).cpp -o $(NAME1).exe $(LDFLAGS) $(LIBS) $(LIBRARIES) 
	$(CC) $(NAME2).cpp -o $(NAME2).exe $(LDFLAGS) $(LIBS) $(LIBRARIES) 

$(NAME1) : $(NAME1).cpp
	$(CC) $(NAME1).cpp -o $(NAME1).exe $(LDFLAGS) $(LIBS) $(LIBRARIES) 

$(NAME2) : $(NAME2).cpp
	$(CC) $(NAME2).cpp -o $(NAME2).exe $(LDFLAGS) $(LIBS) $(LIBRARIES) 


clean:
	touch junk.o junk.exe; rm -f *.o *.exe
