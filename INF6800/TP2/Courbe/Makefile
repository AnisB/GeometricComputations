CC = g++
CFLAGS = -c -I/usr/X11/include -I/opt/local/include
LFLAGS = -L/usr/X11/lib -L/opt/local/lib
LIBS = -lGL -lGLU -lglut
OBJ =  BSpline.o Vector3.o Vector4.o Matrix3x3.o Matrix4x4.o Matrix.o  main.o
TARGET = bspline

all: $(TARGET)

clean:
	-rm -f *.o *~ core *.a

$(TARGET): $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) $(LIBS) -o $(TARGET)

.SUFFIXES: .cpp.o

.cpp.o:
	$(CC) $(CFLAGS) -o $@ $<
