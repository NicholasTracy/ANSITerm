/*
 * ANSITerm Library - Arduino ANSI Terminal Control Library
 * https://github.com/NicholasTracy/ANSITerm
 * 
 * Author: Nicholas Tracy (2024)
 * https://github.com/NicholasTracy
 *
 * File: ANSITerm.h
 * 
 * Description:
 * ANSITerm is an Arduino library designed to provide advanced ANSI escape sequence control 
 * for terminals via the Stream class. This library allows users to create interactive 
 * and graphical terminal interfaces, supporting a wide range of text formatting, color 
 * management, drawing capabilities, and mouse interaction.
 * 
 * The original concept for this library is based on the BasicTerm library by Trannie Carter.
 * https://github.com/nottwo/BasicTerm (2011)
 * 
 * License:
 * This library is licensed under the GNU General Public License v3 (GPLv3).
 * You are free to use, modify, and distribute this library, provided that you comply with 
 * the terms of the GPLv3. There are no warranties, expressed or implied, provided with 
 * this software.
 * 
 * For more details, see the full license at: https://www.gnu.org/licenses/gpl-3.0.en.html
 * 
 */

#ifndef WEB_COLORS_H
#define WEB_COLORS_H

#include <Arduino.h>

// Structure to hold RGB color values
struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

// Web-safe and common named colors
#define COLOR_ALICEBLUE            {240, 248, 255}
#define COLOR_ANTIQUEWHITE         {250, 235, 215}
#define COLOR_AQUA                 {0, 255, 255}
#define COLOR_AQUAMARINE           {127, 255, 212}
#define COLOR_AZURE                {240, 255, 255}
#define COLOR_BEIGE                {245, 245, 220}
#define COLOR_BISQUE               {255, 228, 196}
#define COLOR_BLACK                {0, 0, 0}
#define COLOR_BLANCHEDALMOND       {255, 235, 205}
#define COLOR_BLUE                 {0, 0, 255}
#define COLOR_BLUEVIOLET           {138, 43, 226}
#define COLOR_BROWN                {165, 42, 42}
#define COLOR_BURLYWOOD            {222, 184, 135}
#define COLOR_CADETBLUE            {95, 158, 160}
#define COLOR_CHARTREUSE           {127, 255, 0}
#define COLOR_CHOCOLATE            {210, 105, 30}
#define COLOR_CORAL                {255, 127, 80}
#define COLOR_CORNFLOWERBLUE       {100, 149, 237}
#define COLOR_CORNSILK             {255, 248, 220}
#define COLOR_CRIMSON              {220, 20, 60}
#define COLOR_CYAN                 {0, 255, 255}
#define COLOR_DARKBLUE             {0, 0, 139}
#define COLOR_DARKCYAN             {0, 139, 139}
#define COLOR_DARKGOLDENROD        {184, 134, 11}
#define COLOR_DARKGRAY             {169, 169, 169}
#define COLOR_DARKGREEN            {0, 100, 0}
#define COLOR_DARKKHAKI            {189, 183, 107}
#define COLOR_DARKMAGENTA          {139, 0, 139}
#define COLOR_DARKOLIVEGREEN       {85, 107, 47}
#define COLOR_DARKORANGE           {255, 140, 0}
#define COLOR_DARKORCHID           {153, 50, 204}
#define COLOR_DARKRED              {139, 0, 0}
#define COLOR_DARKSALMON           {233, 150, 122}
#define COLOR_DARKSEAGREEN         {143, 188, 143}
#define COLOR_DARKSLATEBLUE        {72, 61, 139}
#define COLOR_DARKSLATEGRAY        {47, 79, 79}
#define COLOR_DARKTURQUOISE        {0, 206, 209}
#define COLOR_DARKVIOLET           {148, 0, 211}
#define COLOR_DEEPPINK             {255, 20, 147}
#define COLOR_DEEPSKYBLUE          {0, 191, 255}
#define COLOR_DIMGRAY              {105, 105, 105}
#define COLOR_DODGERBLUE           {30, 144, 255}
#define COLOR_FIREBRICK            {178, 34, 34}
#define COLOR_FLORALWHITE          {255, 250, 240}
#define COLOR_FORESTGREEN          {34, 139, 34}
#define COLOR_FUCHSIA              {255, 0, 255}
#define COLOR_GAINSBORO            {220, 220, 220}
#define COLOR_GHOSTWHITE           {248, 248, 255}
#define COLOR_GOLD                 {255, 215, 0}
#define COLOR_GOLDENROD            {218, 165, 32}
#define COLOR_GRAY                 {128, 128, 128}
#define COLOR_GREEN                {0, 128, 0}
#define COLOR_GREENYELLOW          {173, 255, 47}
#define COLOR_HONEYDEW             {240, 255, 240}
#define COLOR_HOTPINK              {255, 105, 180}
#define COLOR_INDIANRED            {205, 92, 92}
#define COLOR_INDIGO               {75, 0, 130}
#define COLOR_IVORY                {255, 255, 240}
#define COLOR_KHAKI                {240, 230, 140}
#define COLOR_LAVENDER             {230, 230, 250}
#define COLOR_LAVENDERBLUSH        {255, 240, 245}
#define COLOR_LAWNGREEN            {124, 252, 0}
#define COLOR_LEMONCHIFFON         {255, 250, 205}
#define COLOR_LIGHTBLUE            {173, 216, 230}
#define COLOR_LIGHTCORAL           {240, 128, 128}
#define COLOR_LIGHTCYAN            {224, 255, 255}
#define COLOR_LIGHTGOLDENRODYELLOW {250, 250, 210}
#define COLOR_LIGHTGRAY            {211, 211, 211}
#define COLOR_LIGHTGREEN           {144, 238, 144}
#define COLOR_LIGHTPINK            {255, 182, 193}
#define COLOR_LIGHTSALMON          {255, 160, 122}
#define COLOR_LIGHTSEAGREEN        {32, 178, 170}
#define COLOR_LIGHTSKYBLUE         {135, 206, 250}
#define COLOR_LIGHTSLATEGRAY       {119, 136, 153}
#define COLOR_LIGHTSTEELBLUE       {176, 196, 222}
#define COLOR_LIGHTYELLOW          {255, 255, 224}
#define COLOR_LIME                 {0, 255, 0}
#define COLOR_LIMEGREEN            {50, 205, 50}
#define COLOR_LINEN                {250, 240, 230}
#define COLOR_MAGENTA              {255, 0, 255}
#define COLOR_MAROON               {128, 0, 0}
#define COLOR_MEDIUMAQUAMARINE     {102, 205, 170}
#define COLOR_MEDIUMBLUE           {0, 0, 205}
#define COLOR_MEDIUMORCHID         {186, 85, 211}
#define COLOR_MEDIUMPURPLE         {147, 112, 219}
#define COLOR_MEDIUMSEAGREEN       {60, 179, 113}
#define COLOR_MEDIUMSLATEBLUE      {123, 104, 238}
#define COLOR_MEDIUMSPRINGGREEN    {0, 250, 154}
#define COLOR_MEDIUMTURQUOISE      {72, 209, 204}
#define COLOR_MEDIUMVIOLETRED      {199, 21, 133}
#define COLOR_MIDNIGHTBLUE         {25, 25, 112}
#define COLOR_MINTCREAM            {245, 255, 250}
#define COLOR_MISTYROSE            {255, 228, 225}
#define COLOR_MOCCASIN             {255, 228, 181}
#define COLOR_NAVAJOWHITE          {255, 222, 173}
#define COLOR_NAVY                 {0, 0, 128}
#define COLOR_OLDLACE              {253, 245, 230}
#define COLOR_OLIVE                {128, 128, 0}
#define COLOR_OLIVEDRAB            {107, 142, 35}
#define COLOR_ORANGE               {255, 165, 0}
#define COLOR_ORANGERED            {255, 69, 0}
#define COLOR_ORCHID               {218, 112, 214}
#define COLOR_PALEGOLDENROD        {238, 232, 170}
#define COLOR_PALEGREEN            {152, 251, 152}
#define COLOR_PALETURQUOISE        {175, 238, 238}
#define COLOR_PALEVIOLETRED        {219, 112, 147}
#define COLOR_PAPAYAWHIP           {255, 239, 213}
#define COLOR_PEACHPUFF            {255, 218, 185}
#define COLOR_PERU                 {205, 133, 63}
#define COLOR_PINK                 {255, 192, 203}
#define COLOR_PLUM                 {221, 160, 221}
#define COLOR_POWDERBLUE           {176, 224, 230}
#define COLOR_PURPLE               {128, 0, 128}
#define COLOR_RED                  {255, 0, 0}
#define COLOR_ROSYBROWN            {188, 143, 143}
#define COLOR_ROYALBLUE            {65, 105, 225}
#define COLOR_SADDLEBROWN          {139, 69, 19}
#define COLOR_SALMON               {250, 128, 114}
#define COLOR_SANDYBROWN           {244, 164, 96}
#define COLOR_SEAGREEN             {46, 139, 87}
#define COLOR_SEASHELL             {255, 245, 238}
#define COLOR_SIENNA               {160, 82, 45}
#define COLOR_SILVER               {192, 192, 192}
#define COLOR_SKYBLUE              {135, 206, 235}
#define COLOR_SLATEBLUE            {106, 90, 205}
#define COLOR_SLATEGRAY            {112, 128, 144}
#define COLOR_SNOW                 {255, 250, 250}
#define COLOR_SPRINGGREEN          {0, 255, 127}
#define COLOR_STEELBLUE            {70, 130, 180}
#define COLOR_TAN                  {210, 180, 140}
#define COLOR_TEAL                 {0, 128, 128}
#define COLOR_THISTLE              {216, 191, 216}
#define COLOR_TOMATO               {255, 99, 71}
#define COLOR_TURQUOISE            {64, 224, 208}
#define COLOR_VIOLET               {238, 130, 238}
#define COLOR_WHEAT                {245, 222, 179}
#define COLOR_WHITE                {255, 255, 255}
#define COLOR_WHITESMOKE           {245, 245, 245}
#define COLOR_YELLOW               {255, 255, 0}
#define COLOR_YELLOWGREEN          {154, 205, 50}

// Function implementation to get color by name
Color getWebColor(const char* name);

#endif // WEB_COLORS_H
