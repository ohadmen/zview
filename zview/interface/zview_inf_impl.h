#pragma once
#include <QSharedMemory>
#include <QSystemSemaphore>
#include "zview/common/common_types.h"
#include "zview_inf.h"
class ZviewInfImpl: public ZviewInf
{
    using Buffer = std::array<std::uint8_t,32U>;
public:
    ZviewInfImpl();
    ~ZviewInfImpl();
    int getLastKeyStroke() override;
    virtual bool getVersion(std::uint8_t* ver) override;
    bool savePly(const char* fn) override;
    
    virtual bool setCameraLookAt(float ex,float ey,float ez,float cx,float cy,float cz,float ux,float uy,float uz) override;
    bool updatePoints(int key,size_t npoints,const float* xyz) override;
    bool updateColoredPoints(int key,size_t npoints,const void* xyzrgba) override;
    int addPoints(const char *name, size_t npoints, const float *xyz) override;
    int addColoredPoints(const char* name,size_t npoints,const void* xyzrgba) override;
    int addMesh(const char* name,size_t npoints,const float* xyz,size_t nfaces,const void* indices) override;
    int addColoredMesh(const char* name,size_t npoints,const void* xyzrgba,size_t nfaces,const void* indices) override;
    int addEdges(const char* name,size_t npoints,const float* xyz,size_t nfaces,const void* indices) override;
    int addColoredEdges(const char* name,size_t npoints,const void* xyzrgba,size_t nfaces,const void* indices) override;
    bool loadFile(const char *filename) override;
    bool removeShape(int key) override;
    void destroy() override;
    int getHandleNumFromString(const char* name) override;
    bool getClickedTarget(float* xyz) override;

    
    static constexpr char INTERFACE_TO_ZVIEW_SHARED_MEM_KEY[] = "zview_from_interface" ;
    static constexpr char ZVIEW_TO_INTERFACE_SHARED_MEM_KEY[] = "zview_to_interface" ;
    static constexpr char INTERFACE_LOCK_KEY[] = "zview_lock" ;
    
    static void initSharedMem(QSharedMemory* data,QSharedMemory* ack);

    enum class Command
    {
        UNKNOWN,
        ADD_PCL,
        ADD_EDGES,
        ADD_MESH,
        REMOVE_SHAPE,
        UPDATE_PCL,
        SAVE_PLY,
        SET_CAM_LOOKAT,
        GET_LAST_KEYSTROKE,
        GET_HNUM_FROM_HSTR,
        GET_TARGET_XYZ,
        GET_VERSION
    };
    struct ReadAck
{
    ZviewInfImpl::Command cmd;
    Buffer buffer;
};

    

private:
    QSharedMemory m_data;
    QSharedMemory m_ack;
    QSystemSemaphore m_lock;


    bool privGetAck(Command expected,Buffer& buffer);
    void privResetAck();
    
};
