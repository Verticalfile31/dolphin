// Copyright 2008 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include <memory>

#include "Common/Assert.h"
#include "Common/GL/GLInterfaceBase.h"
#include "Common/GL/GLUtil.h"
#include "Common/Logging/Log.h"

std::unique_ptr<cInterfaceBase> GLInterface;

namespace GLUtil
{
void InitInterface()
{
  GLInterface = HostGL_CreateGLInterface();
}

GLuint CompileProgram(const std::string& vertexShader, const std::string& fragmentShader)
{
  // generate objects
  GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  GLuint programID = glCreateProgram();

  // compile vertex shader
  const char* shader = vertexShader.c_str();
  glShaderSource(vertexShaderID, 1, &shader, nullptr);
  glCompileShader(vertexShaderID);
#if defined(_DEBUG) || defined(DEBUGFAST)
  GLint Result = GL_FALSE;
  char stringBuffer[1024];
  GLsizei stringBufferUsage = 0;
  glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderInfoLog(vertexShaderID, 1024, &stringBufferUsage, stringBuffer);

  if (Result && stringBufferUsage)
  {
    ERROR_LOG(VIDEO, "GLSL vertex shader warnings:\n%s%s", stringBuffer, vertexShader.c_str());
  }
  else if (!Result)
  {
    ERROR_LOG(VIDEO, "GLSL vertex shader error:\n%s%s", stringBuffer, vertexShader.c_str());
  }
  else
  {
    INFO_LOG(VIDEO, "GLSL vertex shader compiled:\n%s", vertexShader.c_str());
  }

  bool shader_errors = !Result;
#endif

  // compile fragment shader
  shader = fragmentShader.c_str();
  glShaderSource(fragmentShaderID, 1, &shader, nullptr);
  glCompileShader(fragmentShaderID);
#if defined(_DEBUG) || defined(DEBUGFAST)
  glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderInfoLog(fragmentShaderID, 1024, &stringBufferUsage, stringBuffer);

  if (Result && stringBufferUsage)
  {
    ERROR_LOG(VIDEO, "GLSL fragment shader warnings:\n%s%s", stringBuffer, fragmentShader.c_str());
  }
  else if (!Result)
  {
    ERROR_LOG(VIDEO, "GLSL fragment shader error:\n%s%s", stringBuffer, fragmentShader.c_str());
  }
  else
  {
    INFO_LOG(VIDEO, "GLSL fragment shader compiled:\n%s", fragmentShader.c_str());
  }

  shader_errors |= !Result;
#endif

  // link them
  glAttachShader(programID, vertexShaderID);
  glAttachShader(programID, fragmentShaderID);
  glLinkProgram(programID);
#if defined(_DEBUG) || defined(DEBUGFAST)
  glGetProgramiv(programID, GL_LINK_STATUS, &Result);
  glGetProgramInfoLog(programID, 1024, &stringBufferUsage, stringBuffer);

  if (Result && stringBufferUsage)
  {
    ERROR_LOG(VIDEO, "GLSL linker warnings:\n%s%s%s", stringBuffer, vertexShader.c_str(),
              fragmentShader.c_str());
  }
  else if (!Result && !shader_errors)
  {
    ERROR_LOG(VIDEO, "GLSL linker error:\n%s%s%s", stringBuffer, vertexShader.c_str(),
              fragmentShader.c_str());
  }
#endif

  // cleanup
  glDeleteShader(vertexShaderID);
  glDeleteShader(fragmentShaderID);

  return programID;
}

void EnablePrimitiveRestart()
{
  constexpr GLuint PRIMITIVE_RESTART_INDEX = 65535;

  if (GLInterface->GetMode() == GLInterfaceMode::MODE_OPENGLES3)
  {
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
  }
  else
  {
    if (GLExtensions::Version() >= 310)
    {
      glEnable(GL_PRIMITIVE_RESTART);
      glPrimitiveRestartIndex(PRIMITIVE_RESTART_INDEX);
    }
    else
    {
      glEnableClientState(GL_PRIMITIVE_RESTART_NV);
      glPrimitiveRestartIndexNV(PRIMITIVE_RESTART_INDEX);
    }
  }
}
}
