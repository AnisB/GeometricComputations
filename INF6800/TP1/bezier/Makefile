CC = g++
CFLAGS = -c
LFLAGS =
LIBS = -lGL -lGLU -lglut
OBJ =  BezierSpline.o Main.o Matrix.o Matrix3x3.o Matrix4x4.o Vector3.o Vector4.o
TARGET = bezier

all: $(TARGET)

clean:
	-rm -f *.o *~ core *.a

$(TARGET): $(OBJ)
	$(CC) $(LFLAGS) $(OBJ) $(LIBS) -o $(TARGET)

.SUFFIXES: .cpp.o

.cpp.o:
	$(CC) $(CFLAGS) -o $@ $<
