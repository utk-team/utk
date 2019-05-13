#ifndef _RAMP
#define _RAMP

class Ramp
{
private :
    unsigned char m_colors[4][256];
    int m_nodes[256];
    
    public :
    Ramp() 
    { 
        build_thermal();
    } 
    
    ~Ramp() { }
    
public :
    unsigned char* color(unsigned int index) { return m_colors[index]; }
    const unsigned char r(unsigned int index) const { return m_colors[0][index]; }
    const unsigned char g(unsigned int index) const { return m_colors[1][index]; }
    const unsigned char b(unsigned int index) const { return m_colors[2][index]; }
    
    void gl_color(const double value, const double vmax) const
    {
        const unsigned int index = ((unsigned)(value/vmax*255)) % 256;
        ::glColor3ub(r(index),g(index),b(index));
    }
    
private:
    void rebuild()
    {
        // build nodes
        m_colors[3][0] = 1;
        m_colors[3][255] = 1;
        unsigned int nb_nodes = 0;
        for(int i=0;i<256;i++)
        {
            if(m_colors[3][i])
            {
                m_nodes[nb_nodes] = i;
                nb_nodes++;
            }
        }

        // build ramp
        for(int k=0;k<3;k++)
        {   
            for(unsigned int i=0;i<(nb_nodes-1);i++)
            {
                int x1 = m_nodes[i];
                int x2 = m_nodes[i+1];
                int y1 = m_colors[k][x1];
                int y2 = m_colors[k][x2];
                float a = (float)(y2-y1) / (float)(x2-x1);
                float b = (float)y1 - a*(float)x1;
                for(int j=x1;j<x2;j++)
                    m_colors[k][j] = (unsigned char)(a*(float)j+b);
            }
        }
    }
    
    void reset()
    {
        for(int i=1;i<=254;i++)
            m_colors[3][i] = 0;
        m_colors[3][0] = 1;
        m_colors[3][255] = 1;
    }
    
public:
    void build_default()
    {
        reset();
        add_node(0,0,0,0);
        add_node(255,255,255,255);
        rebuild();
    }
    
    void add_node(unsigned int index,
                  unsigned char r,
                  unsigned char g,
                  unsigned char b)
    {
        m_colors[3][index] = 1;
        m_colors[0][index] = r;
        m_colors[1][index] = g;   
        m_colors[2][index] = b;
    }
    
    void build_thermal()
    {
        reset();
        /*
         add_node(0,64,0,0);
         add_node(48,128,0,0);
         add_node(165,255,128,0);
         add_node(255,255,255,255);
        */
        add_node(0,128,0,0);
        add_node(12, 255,0,0);
        add_node(165, 255,128,0);
        add_node(255,255,255,255);
        rebuild();
    }
    
    void build_thermal_foliated()
    {
        build_thermal();
        foliate();
        rebuild();
    }
    
    void foliate()
    {
        int i;
        for(i=10;i<255;i+=10)
        {
            m_colors[3][i-1] = 1;
            m_colors[3][i  ] = 1;
            m_colors[3][i+1] = 1;
            m_colors[0][i] = 100;
            m_colors[1][i] = 100;
            m_colors[2][i] = 100;
        }
    }
    
    void build_rainbow()
    {
        reset();
        add_node(0,0,0,0);
        add_node(32,0,0,255);
        add_node(64,0,255,255);
        add_node(96,0,252,0);
        add_node(140,255,255,0);
        add_node(180,255,128,0);
        add_node(220,255,0,0);
        add_node(240,128,0,0);
        add_node(255,255,255,255);
        rebuild();
    }
};

#endif
