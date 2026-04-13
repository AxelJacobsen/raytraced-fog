*Author:* Axel Elias Wollebekk Jacobsen
# Project topic
My project is a Whitted style raytracing using Blinn-phong for as a local lighting model. Additionally a simple volumetric fog has been implemented to create a moody atmosphere. I intended to create a mineshaft with moving lights to show of the fog, as well as some refractive crystals for a bit of spice. 
## Goals:
1. Raytracing engine
	1. Emission highlights 
	2. Local lighting model
	3. Fresnel refraction
2. Fog
	1. Light color influence
	2. Lighter spatial fog

# Implementation, an anthology
You can skip this and read a more bulletpoint style list further down, this is largely just a walkthrough of what occurred in development. Additionally, i wrote this in obsidian which doesn't permit resizing images which i why i bothered with an appendix.
## First steps
As with most of my projects I have a tendency of diving off the deep end as soon as temporally possible, and this project was no different. After doing some cursory research into what raytracing entails and the different types, as well as what *volumetric* fog actually is, I was off to the races. The general loop of development consisted of "find code which does what I want" -> "extract said niche code" -> "jam it together with the rest until it works". So naturally when following this cycle I started with getting raytracing up and running, which went relatively painless, but this is where my problems started. First of, I knew I wanted a mesh ray tracer, and I knew that would be a massive pain, relative to rendering simple shapes, which showed itself as incomplete shapes and indices errors. I have previous experience with RenderDoc which is a wonderful tool to investigate the data coming from the CPU, but with a comp shader it is slightly lacking in displaying incoming mesh data since everything is drawn as a screen quad. To compensate for this i set up another rendering pipeline for my mesh geometry in a normal vertex-fragment raster shader setup. Here I visualized normals which helped me remain sane during the first week of troubleshooting. [[#1 - Raster rendering]] and [[#2 - Raytraced problems]] are from this period in development.
### Bvh
I expirimented with "smart" bvh setup to increase performance, but i am not "smart", and couldnt get them to work. For that reason i kept it simple and built a single bounding box per mesh. Since i also made all the meshes i just kept them very low poly to compensate. I did want a lamp at some point, but 50 or so vertices bombed my performance so i had to delete it.
## First light
As soon as i solved the initial rendering i built a simple scene in blender and imported the meshes. This went quite quick and smacking in an old phong light model with a single light got me "results". [[#3 - First Light]], though not beautiful, it was certainty a step along the way. 
## Mild hubris pitstop
Around now is a good time to mention that had in fact never checked the delivery date until the week before Easter, notably i was also taking vacation for half of Easter, so at this point i realized that there were some late nights ahead. Regardless, seeing that i had something to look at i decided that im going to need textures and i might as well get that out of the way asap. I used two texture, both from a website called "PolyHaven" which has blender ready textures with normal maps and roughness built in. For loading the textures i spent some time looking around, but ended up confiding in Dr.GPT which gave me some Assimp code for loading a simple texture which i further developed to support normals and roughness. AS OF WRITING THAT CODE SHOULD NOT BE LOOKED AT IF YOU WISH TO MAINTAIN YOUR SAINTY, IT IS BAD AND I CUT AS MANY CORNERS AS POSSIBLE SINCE IT WAS NOT PRIORITY. 

Moving on, since i had gotten most of what i wanted i decided to simply "*Add the fog*", i mean, how hard could it be :). [[#4 - Lights, texture, FOG!]] shows a beautiful scene with fully reflective surfaces, a local lighting model and rays which are bouncing. There is a vignette on the screen which was my pathetic early attempt at fog, this was rather quickly dropped as i realized i did not have the infrastructure for this just yet. Though i would continue to preemptively try my hand at fog, many times. My beautiful lamp can also be spotted in the background by the blue light. Note that at this time i was still using the ambient part of my lighing model, which i would get rid of later since i wanted to restrict all lighting to actual lights. [[#5 - Cool moment in time and a sneak peak at fog]], is the closest i actually got to fog early, but there were too many issues other places that it got discarded during cleaning.
## Cleaning time
One might question how i got this far considering the development process, but the largest contributor was my own willingness to fully throw 80% of my code out the window multiple times during development upon realizing it sucked. [[#6 - Cosine hemisphere and sneaky fog]] shows the first time (from my stored images) where i rendered only the rays which hit an emissive surface/light. In the image i believe i had both a cosine hemisphere function and normal maps implemented which explains the irregular shapes particularly on the wall to the left. A Keen observer might see that there is in fact a poor attempt at fog here as well hiding in the background. Notably this is where i learned the difference in a multisampling-ray marcher and a simple Whitted ray tracer. I knew i wanted to work with a combination setup, but i didn't actually know what it was called or how it worked, learning on the job is the name of the game so to speak.
## Easter
As with all things good, they come at the wrong time. [[#7 - Debugging]] shows the state of affairs as i had to leave for Easter, i was knee deep in figuring out why my scene looked like [[#8 - The problem]]. There are too many issues to cover, but it served as great motivation once i came home.
## More cleaning and real progress
As of 10/4 this was the [[#9 - State of affairs]]. I had made another mesh, a simple crystal, since i knew i wanted some refractive shapes, though its hard to tell in the image, refraction was up and running mostly how it stayed until the end. I cant actually remember the specifics of what i did between 8 and 9, but i do know i did away with cosine hemisphere and used only the normal map since i thought it looked quite a lot nicer. Addressing the elephant in the room, fog is certaintly "there", though it is clearly interacting incorrectly with the lights, though it makes for a very "Aerogel" like substance. This would serve as the bases for all further fog work. 

[[#10 - The great light explosion]] is both a jump to the left and a step to the right, fog was coming along nicely, but as you can see the walls are looking nice and succulent. The fog was causing a massive increase in light, which would cause soaking wet walls until the day of delivery. I believe it was around here i also implemented the roughness map rather than using a simple variable supplied in my magical "meshData" struct. [[#11 - Not so shiny, but twice as pretty]], taken from the same day we can see some things, firstly a noisy fog without banding. As well as some shade coming off of the crystal. Shadowcasting in the fog would haunt my computer as the most intensive work it has done in recent years, but this was somewhat solved in final.
## A fleeting dream
After conferring with my colleague who asked whether or not refractive light would be visible through a given object, i realized that it was in fact not the case. An hour of thinking resulted in a couple schools of though, and none of them were particularly plausible. Since a refracted ray would necessarily end up within the "shadow" of a refractive object i could just do some reverse sampling magic, since i had simple shapes i thought maybe just maybe i could prebake some points which i could later fetch and then reverse-refract back to the light. I did as a matter of fact get this working, by shooting only one beam at the center of each vertex i kept the total number low. [[#12 - A plague]] shows these points, though i had officially gone from 3 Seconds per frame (SPF), to somewhere in the real of 30. I gave up on the idea rather quick as i didn't see the point of spending precious time on it.
## T-minus Two days
With [[#13 - Goal in sight]] i had temporarily disabled fog using a simple flag system i setup. However, we can see some beautiful emissive mesh action, this turned out to be a weirdly large pain in my ass. Since this was an afterthought and a want rather than a need, its in fact just a spherical light source rather than actually using its shape sadly. At this time cosine hemisphere was back as part of a three option reflection rendering tasting project. There are some definite issues with my emission highlights which i couldn't solve before delivery, but i swapped them around and couldn't settle on one i preferred over the other, so i kept them all. 

During [[#14 - Final day]] i spent most of the day cleaning up, squashing any bug i came across and making the scene more presentable. There are still some remaining issues i would like to fix, but time is a cruel mistress. All in all i am relatively happy with the final product. For sure i could keep cranking the fog untill it looked how i wanted and didnt lag, but i think i would rather not :). Since i realized that the crystals didnt sufficiently show refraction i smashed in a sphere using any spare slot of storage i had and the light-intersection function. its a little dark, but i suspect its due to the overall low light levels.

# Implementation 2 The short version
- How does your implementation achieve its goal?
All in all i succeeded in completing the checkpoint list i set as i started the project. There are some issues with fog relating to depth in the frame, but other than that i believe i completed it as well as i could hope. 

- What are some notable problems you encountered on the way? How did you solve them?
What problems did i not encounter :). Due to my erratic development method i pretty much hit every wall possible, but thats how i learn best so its not a bad thing. I solved each problem case by case, if i believed i could formulate the problem in a sensible way i would google and check stack overflow. Otherwise, looking at github for similar code which have solved my issue or finally just feeding my code to Dr. GPT and hoping for the best, though this has mixed results at best.

- What did you find out about the method in terms of its advantages, its limitations, and how to use it effectively?
Well my biggest revelation was that it wasn't just optional to use a local light model for this kind of raytracing, but it was indeed critical and necessary. In my innocent mind back in February i thought i would get most of my scene from direct ray bounces with the local model playing a supporting role, it was indeed the opposite. 
The biggest constraint came from the fog shadowcast which i never solved, i simply had to disable it when working since it caused too much lag. I went through some iterations with simplified intersection functions, but it wasn't viable. 

- Briefly mention what resources did you used to learn about the technique. No need to include every link to everything you read, but I should get a general idea of how you figured it out, even if the answer ends up being pure experimentation!
Stack overflow: Questions, inspiration and various help
Github: Code snippits and inspiration.
LearnOpenGl: Various referencing to brush up on basics. 
ChatGPT: Parsing code from Github which i didnt necesserily understand. 
TA: Help with understanding raytracing and the different types. 
EXPIRIMENTATION: Massive amounts of swapping variables, cranking some numbers and simply feeling things out.
Google / Wikipedia / Other encyclopedias for reading about raytracing and fog.
	Fun fact, Fog doesn't usually appear in caves, I didn't know before starting this project so for continuity this is a fantasy setting. 
# Appendix A
## Texture sources:
Walls: https://polyhaven.com/a/quarry_wall
Supports: https://polyhaven.com/a/rough_wood
# Appendix B / Picture Album <|:)
### 1 - Raster rendering
![[Pasted image 20260412234004.png|Rasterized rendering]]
### 2 - Raytraced problems
![[Pasted image 20260412234010.png|Raytraced problems]]
### 3 - First Light
![[Pasted image 20260412234909.png]]
### 4 - Lights, texture, FOG!
![[Pasted image 20260412235351.png]]
### 5 - Cool moment in time and a sneak peak at fog
![[Pasted image 20260413000744.png]]
### 6 - Cosine hemisphere and sneaky fog
![[Pasted image 20260413001059.png]]
### 7 - Debugging
![[Pasted image 20260413001753.png]]
### 8 - The problem
![[Pasted image 20260413001811.png]]
### 9 - State of affairs
![[Pasted image 20260413002105.png]]
### 10 - The great light explosion
![[Pasted image 20260413002641.png]]

### 11 - Not so shiny, but twice as pretty
![[Pasted image 20260413003243.png]]
### 12 - A plague
![[Pasted image 20260413003525.png]]

### 13 - Goal in sight
![[Pasted image 20260413004105.png]]
### 14 - Final day
![[Pasted image 20260413005455.png]]