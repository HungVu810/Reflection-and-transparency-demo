#version 450 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color_input;
out vec3 color;


uniform float time;

void main(){
    // hypotenuse
    float hyp = sqrt(pow(pos.x, 2) + pow(pos.y, 2));
    float rad = acos(pos.x / hyp);
    /* float rad = pos.x == 0 ? radians(90) : atan(pos.y, pos.x); */
    if(pos.y < 0){ // cos inverse target is [-pi, pi]
        if(pos.x < 0){ // acos gives (pi/2, pi]
            rad += radians(90); // advance from the second to the third quadrant
        }
        else if(pos.x > 0){ // acos gives [0, pi/2)
            rad -= radians(90); // go back from the first to the fourth quadrant
        }
        else { // x = 0, acos gives pi/2
            rad += radians(180);
        }
    }
    gl_Position = vec4(hyp * cos(rad + time), hyp * sin(rad + time), 0.0f, 1.0f);
    /* gl_Position = vec4(pos, 1.0f); */
    color = color_input;
}
