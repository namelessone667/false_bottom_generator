// hello.c
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>

#define PI 3.14159265

double minlength;
bool calculateNewEndPoints(double radius, double startx, double starty, double endx, double endy, double &startx2, double &starty2, double &endx2, double &endy2);

void printElipse2(double basex, double basey, double endx, double endy, double width, std::ofstream &myfile)
{
  double ax, ay, bx, by, cx, cy, dx, dy;
  double side = sqrt(2*(width/2)*(width/2));
  double length = sqrt((endx - basex)*(endx- basex)+(endy- basey)*(endy- basey));
  double angle = asin((endy - basey) / length)  * 180.0 / PI;
  double angle2 = acos((endx - basex) / length) * 180.0 / PI;
  if(angle >=0)
    angle = angle2;
  else
    angle = -angle2;
  ax = endx + cos((angle+180-45) * PI / 180.0)*side;
  ay = endy + sin((angle+180-45) * PI / 180.0)*side;
  bx = endx + cos((angle+180+45) * PI / 180.0)*side;
  by = endy + sin((angle+180+45) * PI / 180.0)*side;
  cx = basex + cos((angle+45) * PI / 180.0)*side;
  cy = basey + sin((angle+45) * PI / 180.0)*side;
  dx = basex + cos((angle-45) * PI / 180.0)*side;
  dy = basey + sin((angle-45) * PI / 180.0)*side;



  //std::cout << "line[" << basex << ", " << basey << "],["<< endx << ","<<endy<<"]\n";
  //std::cout << "length=" << length << " angle=" << angle << " angle2=" << angle2 << " side=" << side << "\n";
  //std::cout << "arc start[" << ax << "," << ay << "] arc end[" << bx << "," << by << "]\n";
  //std::cout << "arc2 start[" << cx << "," << cy << "] arc2 end[" << dx << "," << dy << "]\n";



  myfile << "line\n";
  myfile << ax << "," << ay << "\n";
  myfile << cx << "," <<  cy << "\n";
  myfile << "line\n";
  myfile << bx << "," <<  by << "\n";
  myfile << dx << "," <<  dy << "\n";
  myfile << "arc\n";
  myfile << ax << "," <<  ay  << "\n";
  myfile << endx << "," <<  endy << "\n";
  myfile << bx << "," <<  by << "\n";
  myfile << "arc\n";
  myfile << cx << "," <<  cy << "\n";
  myfile << basex << "," <<  basey << "\n";
  myfile << dx << "," <<  dy << "\n";


}


void printElipse(double basex, double basey, double endx, double width, std::ofstream &myfile)
{
  myfile << "line\n";
  myfile << basex+(width/2) << "," << basey-(width/2) << "\n";
  myfile << endx-(width/2) << "," <<  basey-(width/2) << "\n";
  myfile << "line\n";
  myfile << basex+(width/2) << "," <<  basey+(width/2) << "\n";
  myfile << endx-(width/2) << "," <<  basey+(width/2) << "\n";
  myfile << "arc\n";
  myfile << basex+(width/2) << "," <<  basey-(width/2) << "\n";
  myfile << basex << "," <<  basey << "\n";
  myfile << basex+(width/2) << "," <<  basey+(width/2) << "\n";
  myfile << "arc\n";
  myfile << endx-(width/2) << "," <<  basey-(width/2) << "\n";
  myfile << endx << "," <<  basey << "\n";
  myfile << endx-(width/2) << "," <<  basey+(width/2) << "\n";
  //myfile << "setrelativezero\n";
  //myfile << basex << "," <<  basey << "\n";
}

void printCircle(double centerx, double centery, double radius, std::ofstream &myfile)
{
  myfile << "circle\n";
  myfile << centerx << "," << centery << "\n";
  myfile << centerx << "," << centery + radius << "\n";
}

bool printRow(double x, double y, double length, double width, double offsety, double offsetangle, double centerx, double centery, double radius, std::ofstream &myfile)
{
  bool ret = false;
  bool b1;
  double startx = x, starty = y, endx, endy;
  double fixedx, fixedx2, fixedy, fixedy2;
  while(true)
  {
    endx = startx+length*cos(offsetangle * PI / 180);
    endy = starty+length*sin(offsetangle * PI / 180);
    b1 = calculateNewEndPoints(radius, startx, starty, endx, endy, fixedx, fixedy, fixedx2, fixedy2);

    if( !b1)
      break;
    if(sqrt((fixedx2 - fixedx)*(fixedx2 - fixedx)+(fixedy2 - fixedy)*(fixedy2 - fixedy)) >= minlength)
    {
      printElipse2(fixedx,fixedy,fixedx2, fixedy2,width, myfile);
      ret = true;
    }
    starty+=offsety;

  }

  return ret;
}

void skipComments(std::ifstream &file)
{
  while(true)
  {
    switch(file.peek())
    {
      case 10:
      case 13:
        file.get();
        break;
      case ';':
        file.ignore(256, '\n');
        break;
      default:
        return;
    }

  }
}

bool calculateNewEndPoints(double radius, double startx, double starty, double endx, double endy, double &startx2, double &starty2, double &endx2, double &endy2)
{
  double length = sqrt((endx - startx)*(endx- startx)+(endy- starty)*(endy- starty));
  double angle = asin((endy - starty) / length)  * 180.0 / PI;
  double angle2 = acos((endx - startx) / length) * 180.0 / PI;
  if(angle >=0)
    angle = angle2;
  else
    angle = -angle2;

  double m = tan(angle * PI / 180.0);
  double b = starty - startx * ((endy - starty)/(endx - startx));

  double A = (m*m)+1;
  double B = 2*m*b;
  double C = -(radius*radius)+b*b;
  double BB_4AC = (B*B)-4*A*C;

  if(BB_4AC <= 0)
    return false;

  double sqrtBB_4AC = sqrt(BB_4AC);
  double cx1 = (-1*B+sqrtBB_4AC)/(2*A);
  double cy1 = m*cx1 + b;
  double cx2 = (-1*B-sqrtBB_4AC)/(2*A);
  double cy2 = m*cx2 + b;

  double t1 = (cx1-startx)/(endx-startx);
  double t2 = (cx2-startx)/(endx-startx);

  if(t1>t2)
  {
    double tmpx = cx2, tmpy = cy2, tmpt = t2;
    cx2 = cx1;
    cy2 = cy1;
    cx1 = tmpx;
    cy1 = tmpy;
    t2 = t1;
    t1 = tmpt;
  }

  //std::cout << "intersection1[" << cx1 << ", " << cy1 << "], intersection2["<< cx2 << ","<< cy2<<"]\n";
  //std::cout << "t1=" << t1 << ", t2="<< t2 << "\n";
  //std::cout << "point[" << startx << ", " << starty << "], fixed["<< startx2 << ","<< starty2<<"]\n";
  //std::cout << "point[" << endx << ", " << endy << "], fixed["<< endx2 << ","<< endy2<<"]\n";

  if(t1 <= 0 && t2 >= 1) // line in the boundaries of circle, no fixing needed
  {
    startx2 = startx;
    starty2 = starty;
    endx2 = endx;
    endy2 = endy;
  }
  else if(t1 >= 0 && t1 <= 1 && t2 >= 1) //start is outbound of cicrle, end is inbound
  {
    startx2 = cx1;
    starty2 = cy1;
    endx2 = endx;
    endy2 = endy;
  }
  else if(t1 <= 0 && t2 >= 0 && t2 <= 1) // start is inbound, end is outbound
  {
    startx2 = startx;
    starty2 = starty;
    endx2 = cx2;
    endy2 = cy2;
  }
  else // both start and end are outbound{
  {
    return false;
  }

  return true;
}


int main() {
    double startx = 0, starty = 0;
    double radius, offsetx, offsety, length, width;
    double x = startx, y = starty;
    double border;
    char pattern;
    double angleoffset;

    std::ifstream configFile;
    configFile.open ("falsebottom.cfg", std::ios::in);

    if(!configFile.is_open())
    {
      std::cout << "Failed to open config file falsebottom.cfg";
      return 0;
    }

    skipComments(configFile);
    configFile >> radius;
    if(!configFile.good())
    {
      std::cout << "Config file falsebottom.cfg has invalid format";
      return 0;
    }

    skipComments(configFile);
    configFile >> border;
    if(!configFile.good())
    {
      std::cout << "Config file falsebottom.cfg has invalid format 1";
      return 0;
    }

    skipComments(configFile);
    configFile >> length;
    if(!configFile.good())
    {
      std::cout << "Config file falsebottom.cfg has invalid format 2";
      return 0;
    }

    skipComments(configFile);
    configFile >> angleoffset;
    if(!configFile.good())
    {
      std::cout << "Config file falsebottom.cfg has invalid format 7";
      return 0;
    }

    skipComments(configFile);
    configFile >> width;
    if(!configFile.good())
    {
      std::cout << "Config file falsebottom.cfg has invalid format 3";
      return 0;
    }

    skipComments(configFile);
    configFile >> offsetx;
    if(!configFile.good())
    {
      std::cout << "Config file falsebottom.cfg has invalid format 4";
      return 0;
    }

    skipComments(configFile);
    configFile >> offsety;
    if(!configFile.good())
    {
      std::cout << "Config file falsebottom.cfg has invalid format 5";
      return 0;
    }

    skipComments(configFile);
    configFile >> minlength;
    if(!configFile.good())
    {
      std::cout << "Config file falsebottom.cfg has invalid format 6";
      return 0;
    }

    if(angleoffset > 75 || angleoffset < -75)
    {
      std::cout << "row angle offset should be between -75 and 75 degree";
      return 0;
    }

    std::ofstream myfile;
    myfile.open ("librecad.txt", std::ios::trunc);

    printCircle(x,y,radius, myfile);

    bool b1, b2;
    bool reverse = false;
    double startangle = angleoffset;

    while(true)
    {
      b1 = printRow(x+offsetx/2,y,length, width, offsety+width, angleoffset, startx, starty, radius-border, myfile);
      b2 = printRow(x+offsetx/2,y - offsety-width,length, width, -offsety-width, angleoffset, startx, starty, radius-border, myfile);

      if(!b1 && !b2)
      {
        if(reverse)
          break;
        else
        {
          reverse = true;
          x = startx;
          y = starty;
          length *= -1;
          offsetx *= -1;
          angleoffset = startangle * -1;
          continue;
        }

      }

      x += offsetx + length*cos(angleoffset * PI / 180);
      y = starty;
      angleoffset *= -1;
    }



    myfile.close();
    return 0;
}
