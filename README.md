# Procedural-generation

This repo contains a game implemented on top of an old version of my custom engine. 
Although I never really named it officialy, I like to call it "Trial of the elements", because I'm like that.

The game flows across four different levels, each with a different gameplay mechanic. In a way, it is a puzzle game.

The first level, trial of earth, is a procedurally generate maze (Eller's algorithm), in which the player has to find the 
lord of the realm - the earth sprite. 
This level features maze generation, geometry generation, normal mapping and a particle system 
(albeit used to animate a persisten entity, not temporary particles).

The second level, trial of fire, is about guile and speed. One must follow a path of hexagonal floating stones, which dynamically 
appear above a lava lake one at a time in a pseudo random fashion. Fail to identify which one spawned next and walking over to it, 
an you will quickly find yourself in lava. The goal is to reach the fire sprite, represented by a volumetric fire effect.
This level features procedural terrain generation, blur and bloom, a tree generated by L-systems with normal-mapped bark, 
a random path generator between specified points and volumetric rendering.

During the third level, designed around water, the player must be perceptive and observant.
This level, just like the fire level, features dynamic collision and L-systems (pretty flower in the middle) as well. 
Fresnel effect, plane-clipped reflections and dynamic normals are implemented to breathe life into the water surface.
Cube map reflections, for the sphere overhead, and the volumetric water sprite wrap up the list of techniques within this level. 

The final level, trial of air, changes from first person to third person perspective. Players no longer control this invisible entity, 
and instead find themselves operating a glider. They need to catch the elusive lord of the skies thrice, while not crashing into terrain. 
And what is an adventure without an occasional monster? A dragon, construced of spheres (very ominous spheres mind you),
procedurally shaded, held together by the magic of an evil wiz... by a spring mass system, actually... chases the player. 
If the player drops too low in height, they need only enter the gust of air (volumetric effect) in the middle of the map that will 
lift them upwards. Terain is procedurally generated here as well.

Making this game was a wild experience but very fun. Most of the functionality was implemented bottom up by yours truly, 
with a little help from the following.

Credit where it's due:
  DirectXTK, for the aptly named SimpleMath, because rolling your own matrix library hurts. 
  Assimp, for making the best out of the willy nilly gathered collection of freely available models and textures.
  Sean Barret for the amazing STB library!
  Fine people at Sketchfab and Turbosquid that provided the art.
  Jorge L. Rodriguez for his informative videos that made learning 3D game specific math such a pleasure.

And of course, I thank you, my friends and family, for making the grind worth it.
