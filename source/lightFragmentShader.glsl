#version 330 core

out vec3 colour;

uniform vec3 lightColour;

void main ()
{
    colour = lightColour;
}
