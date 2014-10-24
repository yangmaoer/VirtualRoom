//#############################################################################
//  File:      TextureOnly.frag
//  Purpose:   GLSL fragment shader for texture mapping only
//  Author:    Marcus Hudritsch
//  Date:      July 2014
//  Copyright: 2002-2014 Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D u_texture0;    // Color map
varying vec2      v_texCoord;    // Interpol. texture coordinate

void main()
{     
    gl_FragColor = texture2D(u_texture0, v_texCoord);
}