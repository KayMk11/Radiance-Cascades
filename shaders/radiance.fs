#version 330 core

out vec4 FragColor;
in vec2 UV;

uniform sampler2D sdfTex;
uniform sampler2D lastCascade;

uniform ivec2 cascadeLevel;

float epsilon = 0.01;
vec2 resolution = vec2(800, 800);
vec2 invResolution = 1.0/resolution;
float TAU=6.28318531;

vec3 computeRC(){
    
    vec2 scale=800*invResolution;
    
    vec2 coord=floor(UV*resolution);
    
    int baseRayCount=4;
    float sqrtBase=2.0;
    float radianceInc=0.25;
    
    int rayCount=int(pow(float(baseRayCount),float(cascadeLevel.x)));
    
    
    float spacing=pow(sqrtBase,float(cascadeLevel.x-1));
    
    vec2 size=floor(resolution/spacing);
    vec2 probeRelativePosition=mod(coord,size);
    
    vec2 rayPos=floor(coord/size);
    
    float baseIndex=float(baseRayCount)*(rayPos.x+(spacing*rayPos.y));
    
    vec2 probeCenter=(probeRelativePosition+0.5)*spacing;
    vec2 normalizedProbeCenter=probeCenter/resolution;
    
    float shortestSide=0.75*(1.0/800);
    
    float startLength=pow(float(baseRayCount),float(cascadeLevel.x-1))*shortestSide;
    float maxLength=pow(float(baseRayCount),float(cascadeLevel.x))*shortestSide;
    
    vec3 col=vec3(0);
    float numRays=float(rayCount);
    float angleStepSize=TAU/numRays;
    
    
    for(int i=0;i<baseRayCount;i++){
        
        float index=baseIndex+float(i);
        float angleStep=index+0.5;
        float angle=angleStepSize*angleStep;
        
        vec2 dir=normalize(vec2(cos(angle),-sin(angle)))*scale;
        vec2 pos=normalizedProbeCenter+dir*startLength;
        
        float traveled=startLength;
        
        bool hitSmth=false;
        
        for(int j=0;j<30;j++){
            
            vec4 sam=texture(sdfTex,pos);
            sam.w*=(255.0/800.0);
            pos+=dir*sam.w;
            
            if(pos.x<0.0||pos.x>1.0||pos.y<0.0||pos.y>1.0)
            {
                col += vec3(0, 0, .1);
                break;
            }
                
            if(sam.w<epsilon){
                
                col += vec3(0, .1, 0);
                col+=(pow(texture(sdfTex,pos).rgb,vec3(2.2))* vec3(0,1,0));
                hitSmth=true;
                break;
            }
            
            traveled+=sam.w;
            
            if(traveled>maxLength)
                break;
        }
        
        if(cascadeLevel.x<cascadeLevel.y&&!hitSmth){
            
            col =vec3(.1,0,0);
            float upperSpacing=pow(sqrtBase,float(cascadeLevel.x));
            vec2 upperSize=floor(resolution/upperSpacing);
            vec2 upperPosition = vec2(mod(index,upperSpacing),floor(index/upperSpacing))*upperSize;
            
            vec2 offset=(probeRelativePosition+0.5)/sqrtBase;
            // vec2 clamped=clamp(offset,vec2(0.5),upperSize-0.5);
        
            col+=pow(texture(lastCascade,(upperPosition+offset)*invResolution).rgb,vec3(2.2)) * vec3(1,0,0);
        }
    }
    
    return col*radianceInc;
}


void main(){
    
    // vec4 sdf=texture(sdfTex,UV);
    // vec3 renderCol=sdf.w-0.02>0.0?vec3(0):sdf.rgb;
    
    // if(sdf.w-0.02<0.0){
        
    //     FragColor=vec4(renderCol,1);
    // }else{
    if(!(cascadeLevel.x>1)){
        FragColor=vec4(computeRC(),1);
    }else{
        FragColor=vec4(pow(computeRC(),vec3(1.0/2.2)),1);
    }
    // }
    
}