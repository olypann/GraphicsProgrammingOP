
////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/maths.hpp>
#include <common/camera.hpp>
#include <common/model.hpp>
#include <common/light.hpp>

void keyboardInput(GLFWwindow *window);
void mouseInput(GLFWwindow *window);

// time of previous iteration of the loop and time elapsed since the previous frame
float previousTime = 0.0f;  
float deltaTime    = 0.0f;  

float lightBlinkTimer = 0.0f;
bool lightBlinking = false;
glm::vec3 blinkingLightColor(1.0f, 0.87f, 0.7f);

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f));

bool isFirstPerson = true;

bool switchedToFirstPerson = false;


struct Object
{
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 scale    = glm::vec3(1.0f, 1.0f, 1.0f);
    float angle = 0.0f;
    std::string name;
};

Object* player = nullptr;
std::vector<Object> objects;



int main( void )
{
    float activationRadius = 5.0f; 
    bool lightActivated = false;   
    Object activatedCheese;       

    
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    
    GLFWwindow* window;
    window = glfwCreateWindow(1024, 768, "coursework", NULL, NULL);
    
    if( window == NULL ){
        fprintf(stderr, "Failed to open GLFW window.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();

        glfwTerminate();

        return -1;
    }
    
    glEnable(GL_DEPTH_TEST);
    
    
    glEnable(GL_CULL_FACE);
    
    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);
    
    unsigned int shaderID, lightShaderID;
    shaderID      = LoadShaders("vertexShader.glsl", "fragmentShader.glsl");
    lightShaderID = LoadShaders("lightVertexShader.glsl", "lightFragmentShader.glsl");
    
    glUseProgram(shaderID);
    
    Model cheese("../assets/cheese.obj");
    Model sphere("../assets/sphere.obj");

    Model rat("../assets/rat.obj");
    
    cheese.addTexture("../assets/yellow.bmp", "diffuse");
    cheese.addTexture("../assets/diamond_normal.png", "normal");
    cheese.addTexture("../assets/neutral_specular.png", "specular");

    rat.addTexture("../assets/fur.bmp", "diffuse");
    rat.addTexture("../assets/diamond_normal.png", "normal");
    rat.addTexture("../assets/neutral_specular.png", "specular");
    
    cheese.ka = 0.2f;
    cheese.kd = 0.7f;
    cheese.ks = 1.0f;
    cheese.Ns = 20.0f;

    rat.ka = 0.2f;
    rat.kd = 0.7f;
    rat.ks = 1.0f;
    rat.Ns = 20.0f;



    
    Light lightSources;

    //central light of the scene


    lightSources.addPointLight(
        glm::vec3(0.0f, 2.0f, 0.0f),          
        glm::vec3(1.0f, 0.87f, 0.7f),          
        1.0f, 0.05f, 0.02f                    
    );


    lightSources.addSpotLight(
        glm::vec3(-5.0f, 3.0f, -5.0f),        
        glm::vec3(0.0f, -1.0f, 0.0f),         
        glm::vec3(1.0f, 0.0f, 0.0f),          
        1.0f, 0.1f, 0.02f,
        std::cos(Maths::radians(30.0f))        
    );

    lightSources.addSpotLight(
        glm::vec3(5.0f, 3.0f, -5.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),           
        1.0f, 0.1f, 0.02f,
        std::cos(Maths::radians(30.0f))
    );

    lightSources.addSpotLight(
        glm::vec3(-5.0f, 3.0f, 5.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),           
        1.0f, 0.1f, 0.02f,
        std::cos(Maths::radians(30.0f))
    );

    lightSources.addSpotLight(
        glm::vec3(5.0f, 3.0f, 5.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 0.0f),           
        1.0f, 0.1f, 0.02f,
        std::cos(Maths::radians(30.0f))
    );

    ///

    glm::vec3 spotlightPosition(0.0f, 5.0f, 0.0f); 
    glm::vec3 spotlightColor(1.0f, 1.0f, 1.0f); 
    float spotlightRadius = 10.0f; 
    
    

    glm::vec3 cheesePositions[] = {
        glm::vec3(-5.0f, 0.0f, -5.0f),  
        glm::vec3(5.0f, 0.0f, -5.0f),  
        glm::vec3(-5.0f, 0.0f,  5.0f), 
        glm::vec3(5.0f, 0.0f,  5.0f)   
    };



    
    Object object;
    object.name = "cheese";
    
    for (unsigned int i = 0; i < 4; i++) {
        object.position = cheesePositions[i];
        object.rotation = glm::vec3(1.0f, 1.0f, 1.0f);
        object.scale = glm::vec3(0.75f, 0.75f, 0.75f);
        object.angle = Maths::radians(20.0f * i);
        objects.push_back(object);
    }



    Model floor("../assets/plane.obj");
    floor.addTexture("../assets/stones_diffuse.png", "diffuse");
    floor.addTexture("../assets/stones_normal.png", "normal");
    floor.addTexture("../assets/stones_specular.png", "specular");

    floor.ka = 0.2f;
    floor.kd = 1.0f;
    floor.ks = 1.0f;
    floor.Ns = 20.0f;

    object.position = glm::vec3(0.0f, -0.85f, 0.0f);
    object.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    object.rotation = glm::vec3(0.0f, 1.0f, 0.0f);
    object.angle = 0.0f;
    object.name = "floor";

    objects.push_back(object);



    object.name = "rat";
    object.position = glm::vec3(1.0f, 1.0f, 1.0f);
    object.rotation = glm::vec3(1.0f, 1.0f, 1.0f);
    object.scale = glm::vec3(0.75f, 0.75f, 0.75f);
    object.angle = Maths::radians(0.0f);
    objects.push_back(object);
    for (auto& obj : objects) {
        if (obj.name == "rat") {
            player = &obj;
            break;
        }
    }
    
    /////////////////////

    
    while (!glfwWindowShouldClose(window))
    {
        float time   = glfwGetTime();
        deltaTime    = time - previousTime;
        previousTime = time;
        
        keyboardInput(window);
        mouseInput(window);
        
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        

        // adjusting based on the player's actual position
        glm::vec3 playerPosition(2.0f, 1.6f, 2.0f); 
        float distance = glm::length(playerPosition - spotlightPosition);
        float intensity = glm::clamp(1.0f - (distance / spotlightRadius), 0.0f, 1.0f);
        spotlightColor = glm::mix(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.5f, 0.0f), intensity);



        
        camera.calculateMatrices();
        
        float triggerRadius = 2.5f;
        float floatSpeed = 1.0f;
        float maxFloatHeight = 2.0f;

        for (auto& obj : objects) {


            if (obj.name == "cheese") {
                glm::vec3 playerPos = isFirstPerson ? camera.eye : player->position;
                float distance = glm::length(obj.position - playerPos);

                if (distance < triggerRadius) {
                    if (obj.position.y < maxFloatHeight) {
                        obj.position.y += floatSpeed * deltaTime;
                        if (obj.position.y > maxFloatHeight)
                            obj.position.y = maxFloatHeight;
                    }
                    if (!lightBlinking) {
                        lightBlinking = true;
                        lightBlinkTimer = 2.0f;
                    }
                }
                else {
                    float groundY = 0.0f;
                    if (obj.position.y > groundY) {
                        obj.position.y -= floatSpeed * deltaTime;
                        if (obj.position.y < groundY)
                            obj.position.y = groundY;
                    }
                }
            }
        }

        


        
        if (isFirstPerson) {
            /*if (player != nullptr) {
                camera.eye = player->position + glm::vec3(0.0f, 1.6f, 0.0f);
            }*/

            // first-person quaternion view
            camera.quaternionCamera(); 
        }
        else {
            if (player != nullptr) {
                // third-person offset quaternion view
                float distance = 5.0f;
                float height = 2.0f;
                float yaw = camera.yaw;

                glm::vec3 offset = glm::vec3(
                    -distance * std::cos(yaw),
                    height,
                    -distance * std::sin(yaw)
                );

                camera.eye = player->position + offset;
                camera.target = player->position;
                camera.calculateMatrices();

                

                player->rotation = glm::vec3(0.0f, 1.0f, 0.0f); 
                player->angle = -yaw;
            }
        }


        glUseProgram(shaderID);





        lightSources = Light();
        if (lightBlinking) {
            lightBlinkTimer -= deltaTime;

            bool lightOn = fmod(glfwGetTime(), 0.6f) < 0.3f; 
            blinkingLightColor = lightOn ? glm::vec3(1.0f, 0.87f, 0.7f) : glm::vec3(0.0f);

            if (lightBlinkTimer <= 0.0f) {
                lightBlinking = false;
                blinkingLightColor = glm::vec3(1.0f, 0.87f, 0.7f); 
            }
        }
        lightSources.addPointLight(
            glm::vec3(0.0f, 2.0f, 0.0f),
            blinkingLightColor,
            1.0f, 0.05f, 0.02f
        );


        lightSources.addSpotLight(
            glm::vec3(-5.0f, 3.0f, -5.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f), 
            1.0f, 0.1f, 0.02f,
            std::cos(Maths::radians(30.0f))
        );

        lightSources.addSpotLight(
            glm::vec3(5.0f, 3.0f, -5.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f), 
            1.0f, 0.1f, 0.02f,
            std::cos(Maths::radians(30.0f))
        );

        lightSources.addSpotLight(
            glm::vec3(-5.0f, 3.0f, 5.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f), 
            1.0f, 0.1f, 0.02f,
            std::cos(Maths::radians(30.0f))
        );

        lightSources.addSpotLight(
            glm::vec3(5.0f, 3.0f, 5.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 0.0f), 
            1.0f, 0.1f, 0.02f,
            std::cos(Maths::radians(30.0f))
        );

        
        lightSources.toShader(shaderID, camera.view);
        
        for (unsigned int i = 0; i < static_cast<unsigned int>(objects.size()); i++)
        {
            glm::mat4 translate = Maths::translate(objects[i].position);
            glm::mat4 scale     = Maths::scale(objects[i].scale);
            glm::mat4 rotate    = Maths::rotate(objects[i].angle, objects[i].rotation);
            glm::mat4 model     = translate * rotate * scale;
            
            glm::mat4 MV  = camera.view * model;
            glm::mat4 MVP = camera.projection * MV;
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "MV"), 1, GL_FALSE, &MV[0][0]);
            
            if (objects[i].name == "cheese")
                cheese.draw(shaderID);

            if (objects[i].name == "floor")
                floor.draw(shaderID);

            

            if (objects[i].name == "rat") {
                if (!isFirstPerson) 
                    rat.draw(shaderID);
            }


        }
        
        lightSources.draw(lightShaderID, camera.view, camera.projection, sphere);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // clean up
    cheese.deleteBuffers();
    rat.deleteBuffers();

    glDeleteProgram(shaderID);
    
    glfwTerminate();
    return 0;
}

void keyboardInput(GLFWwindow *window)
{

    float activationRadius = 5.0f; 
    bool lightActivated = false;   
    Object activatedCheese;      




    static bool spacePressedBefore = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    bool spacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    if (spacePressed && !spacePressedBefore)
        isFirstPerson = !isFirstPerson;
        switchedToFirstPerson = true;

     /*if (switchedToFirstPerson) {
            camera.eye = player->position + glm::vec3(0.0f, 1.6f, 0.0f);
            switchedToFirstPerson = false;
     }*/
        
        
    spacePressedBefore = spacePressed;

    float speed = 5.0f * deltaTime;



    if (isFirstPerson) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.eye += speed * camera.front;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.eye -= speed * camera.front;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.eye -= speed * camera.right;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.eye += speed * camera.right;

        float groundY = -0.85f + 0.75f; 

        camera.eye.y = groundY;


    }
    else {
        if (player != nullptr) {
            glm::vec3 direction;
            float yaw = camera.yaw;
            glm::vec3 forward = glm::normalize(glm::vec3(
                std::cos(yaw), 0.0f, std::sin(yaw)
            ));
            glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                player->position += speed * forward;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                player->position -= speed * forward;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                player->position -= speed * right;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                player->position += speed * right;


            float groundY = -0.85f + 0.75f; 

            player->position.y = groundY;


        }
    }

    /////

    glm::vec3 playerMin = camera.eye - glm::vec3(0.25f, 0.0f, 0.25f);
    glm::vec3 playerMax = camera.eye + glm::vec3(0.25f, 1.8f, 0.25f);

    for (const Object& obj : objects) {
        if (obj.name == "cheese") {
            glm::vec3 cheeseMin = obj.position - obj.scale * 0.5f;
            glm::vec3 cheeseMax = obj.position + obj.scale * 0.5f;

            bool collisionX = playerMax.x >= cheeseMin.x && playerMin.x <= cheeseMax.x;
            bool collisionY = playerMax.y >= cheeseMin.y && playerMin.y <= cheeseMax.y;
            bool collisionZ = playerMax.z >= cheeseMin.z && playerMin.z <= cheeseMax.z;

            if (collisionX && collisionY && collisionZ) {
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                    camera.eye -= 5.0f * deltaTime * camera.front;
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                    camera.eye += 5.0f * deltaTime * camera.front;
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                    camera.eye += 5.0f * deltaTime * camera.right;
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                    camera.eye -= 5.0f * deltaTime * camera.right;

                break;
            }
        }
    }
}


void mouseInput(GLFWwindow *window)
{
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    float xOffset = float(xPos - 1024 / 2);
    float yOffset = float(768 / 2 - yPos);
    
    camera.yaw   += 0.003f * float(xPos - 1024 / 2);
    camera.pitch += 0.003f * float(768 / 2 - yPos);

    if (camera.pitch > 1.5f) camera.pitch = 1.5f;
    if (camera.pitch < -1.5f) camera.pitch = -1.5f;
    
    camera.calculateCameraVectors();
}



//my own functions own functions to replace glm functions
//function to normalize a vector (make it a unit vector)
glm::vec3 normalize(const glm::vec3& v) {
    float len = length(v);
    if (len > 0) {
        return glm::vec3(v.x / len, v.y / len, v.z / len);
    }
    return glm::vec3(0, 0, 0); // Return a zero vector if length is 0
}

//check if an object is in front of the player
bool isInFront(const glm::vec3& viewDir, const glm::vec3& targetDir) {
    return glm::dot(glm::normalize(viewDir), glm::normalize(targetDir)) > 0.0f;
}

//check if camera rays intersect with an object (if object is in view)
bool rayIntersectsSphere(
    const glm::vec3& rayOrigin,
    const glm::vec3& rayDir,
    const glm::vec3& sphereCenter,
    float radius
) {
    glm::vec3 L = sphereCenter - rayOrigin;
    float tca = glm::dot(L, glm::normalize(rayDir));
    float d2 = glm::dot(L, L) - tca * tca;
    return d2 <= radius * radius;
}


glm::mat4 LookAtPersonal(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
    glm::vec3 zaxis = glm::normalize(eye - center);

    glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(up), zaxis));

    glm::vec3 yaxis = glm::cross(zaxis, xaxis);

    glm::mat4 view(1.0f);
    view[0][0] = xaxis.x;
    view[1][0] = xaxis.y;
    view[2][0] = xaxis.z;

    view[0][1] = yaxis.x;
    view[1][1] = yaxis.y;
    view[2][1] = yaxis.z;

    view[0][2] = zaxis.x;
    view[1][2] = zaxis.y;
    view[2][2] = zaxis.z;

    view[3][0] = -glm::dot(xaxis, eye);
    view[3][1] = -glm::dot(yaxis, eye);
    view[3][2] = -glm::dot(zaxis, eye);


    return view;
}


glm::mat4 projectionPersonal(float left, float right, float bottom, float top, float near, float far) {
    glm::mat4 result(1.0f);
    result[0][0] = 2.0f / (right - left);
    result[1][1] = 2.0f / (top - bottom);
    result[2][2] = -2.0f / (far - near);

    result[3][0] = -(right + left) / (right - left);
    result[3][1] = -(top + bottom) / (top - bottom);
    result[3][2] = -(far + near) / (far - near);


    return result;
}