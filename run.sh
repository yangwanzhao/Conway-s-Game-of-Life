g++ life.cc \
	-std=c++11 \
	-lopencv_core \
	-lopencv_imgproc \
	-lopencv_imgcodecs \
	-lopencv_videoio \
	-pthread \
	-ltbb \
	-o gamelife

# ./gamelife -i "born1.txt" -o "life.mp4" 



# -i "exa.jpg" -o "exa_outp.jpg" -f "exa_outv.mp4"