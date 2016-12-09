This program was made by Mitchell Hewitt for CS430 Computer Graphics (Section 1), Project 5 - Image Viewer, in Fall 2016
It is an extension of the demo.c file by Dr. James Palmer at https://bitbucket.org/jdpalmer/gles2-starter-kit
This program reads in an input P3 .ppm file.  
It stores the image's data and maps that image data to two triangles made of four vertices.
These two textured triangles are rendered, and the rendered triangles may be modified based on keyboard input.

To use this program...

	1.  Compile it with the Visual Studio Command Prompt using 'nmake' in the main folder of the GLES2 Starter Kit.

	2.  Use the command "ezview input.ppm" to read the input p3 .ppm file
	     and display it as a texture on two triangles in a window.
		 
    3. Keyboard shortcuts:
	    Rotation: Q to rotate counter-clockwise
		          W to rotate clockwise
				  
		Scaling:  A to scale the image up.
		          S to scale the image down.
				  
	    Shearing: Z to shear the image right  (with respect to to the original vertex orientation)
		          X to shear the image left (with respect to the original vertex orientation)
				  
		Panning:  Left arrow key to pan the image left.
		          Right arrow key to pan the image right.
				  Up arrow key to pan the image up.
				  Down arrow key to pan the image down.
				  
		Reset:    Spacebar to set the vertices to their original locations.
		Close:    Escape to close the program.

If you would like to verify the texturing...

	1.  Open input.ppm with software that can open .ppm (P3) files (e.g. GIMP)

	2.  Observe that...
            The pepper is in the top left
			The top portion of the image is blue
			The word "Image" is black, in the center of the image.
	
	3.  In the program (upon initially running it or after pressing spacebar) observe that...
            The pepper is in the top left
			The top portion of the image is blue
			The word "Image" is black, in the center of the image.
			
	4.  Utilize the keyboard controls outlined above and verify they performed the transformations described above.
	

**NOTE:** When I ran the program, the memory where the image's width was stored sometimes had an extra number not present 
         in the image file.  Running and closing the program until this extra number is absent may be necessary.
		 
Invalid inputs and file contents will close the program.
This program is designed to use eight bits per color channel.