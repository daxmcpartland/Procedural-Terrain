#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 pos;

uniform sampler2D water;
uniform sampler2D sand;
uniform sampler2D soil;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D snow;

void main()
{    
	float waterThreshold = -1.0;
	float sandThreshold = 0.0f;
	float soilThreshold = 1.5;
	float grassThreshold = 4.0;
	float rockThreshold = 7.0;
	float snowThreshold = 8.5;
	float height = pos.y;
	if(height == waterThreshold)
	{
		FragColor = texture(water, TexCoords);
	}
	else if(height < sandThreshold)
	{
		float rate = (height - waterThreshold)/(sandThreshold - waterThreshold);
		FragColor = mix(texture(water, TexCoords), texture(sand, TexCoords), rate);
	}
	else if(height < soilThreshold)
	{
		float rate = (height - sandThreshold)/(soilThreshold - sandThreshold);
		FragColor = mix(texture(sand, TexCoords), texture(soil, TexCoords), rate);
	}
	else if(height < grassThreshold)
	{
		float rate = (height - soilThreshold)/(grassThreshold - soilThreshold);
		FragColor = mix(texture(soil, TexCoords), texture(grass, TexCoords), rate);
	}
	else if(height < rockThreshold)
	{
		float rate = (height - grassThreshold)/(rockThreshold - grassThreshold);
		FragColor = mix(texture(grass, TexCoords), texture(rock, TexCoords), rate);
	}
	else if(height < snowThreshold)
	{
		float rate = (height - rockThreshold)/(snowThreshold - rockThreshold);
		FragColor = mix(texture(rock, TexCoords), texture(snow, TexCoords), rate);
	}
	else
	{
		FragColor = texture(snow, TexCoords);
	}
}