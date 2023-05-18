# Julia render
Simple little julia render that I used to create a very lovely animation. To see the results of that, see https://youtu.be/6tMu-61DL7I or https://youtu.be/3ZkzaIJ\_\_7o.

# Usage
1. Edit src/main.c and change the constants as desired. **N.B: CHANGE THE `THREADS` VALUE TO THE AMMOUNT OF CORES YOU HAVE OR SUFFER**. Modify how the colours array is filled in for nicer colours.
2. run `make`
3. run `./julia`
  1. **OPTIONAL**: Run `./julia &` instead to do it in the background instead
  2. **OPTIONAL**: Run `make renice` to give the process slightly more favourable CPU usage (Might bring your computer to a crawl)
4. Once the program is done, use `make render` (Or `make nvrender` to use CUDA on an NVidia graphics card) to render a video file. 
5. **OPTIONAL**: Use `make small` (Or `make nvsmall` to use CUDA on an NVidia graphics card) to compress the video into a more standard video size

# Improvements
- A possible improvement would be to move the Julia renderer to the GPU using GLSL or CUDA or something but I decided just to hurt my CPU instead. 
- Something else to improve would be to find a way to avoid using SDL in favour of something a little faster. STBI renderer the PNGs a little too slow so I stuck with SDL instead.
- Another good idea would be to figure out a format to replace PNG. Compressing the PNGs is costly. The issue is that disk space is limited and PNGs compress the best without losing detail in the image. Maybe making some sort of "on-the-fly" stitching together of the images so that only the video remains but I was too lazy to do this.

# Colours
The colour palette is taken from https://github.com/HackerPoet/FractalSoundExplorer. It is pre-rendered instead as to make it *slightly* faster.

# License
GPL-v3.0 :)
