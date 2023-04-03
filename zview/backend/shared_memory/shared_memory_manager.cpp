#include "shared_memory_manager.h"
#include "zview/gui/drawables/drawables_buffer.h"
#include "zview/common/mem_stream.h"
#include "cmd_query_thread.h"
#include <QThread>
#include <QDebug>



std::string SharedMemoryManager::privReadName(const char *dataptr, size_t *offsetP) const
{
    std::string name(dataptr + *offsetP);
    *offsetP += name.size() + 1;
    return name;
}
template <class T>
std::vector<T> privReadvector(const char *dataptr, size_t *offsetP)
{
    size_t sz;
    memcpy(&sz, dataptr + *offsetP, sizeof(size_t));
    std::vector<T> vect(sz);
    memcpy(&vect[0], dataptr + *offsetP + sizeof(size_t), sizeof(T) * sz);
    *offsetP += sz * sizeof(T) + sizeof(size_t);
    return vect;
}
qint64 privReadShape(ConstMemStream &ms, ZviewInfImpl::Command cmd)
{
    std::string name;
    size_t pclSz;
    ms >> name >> pclSz;
    std::vector<Types::VertData> pcl(pclSz);
    ms >> pcl;

    switch (cmd)
    {
    case ZviewInfImpl::Command::ADD_PCL:
    {
        Types::Pcl obj(name);
        obj.v() = std::move(pcl);

        return drawablesBuffer.addShape(std::move(obj));
    }

    case ZviewInfImpl::Command::ADD_EDGES:
    {
        size_t edgesSz;
        ms >> edgesSz;
        std::vector<Types::EdgeIndx> edges(edgesSz);
        ms >> edges;
        Types::Edges obj(name);
        obj.v() = std::move(pcl);
        obj.e() = std::move(edges);
        return drawablesBuffer.addShape(std::move(obj));
    }
    case ZviewInfImpl::Command::ADD_MESH:
    {
        size_t facesSz;
        ms >> facesSz;
        std::vector<Types::FaceIndx> faces(facesSz);
        ms >> faces;
        Types::Mesh obj(name);
        obj.v() = std::move(pcl);
        obj.f() = std::move(faces);
        return drawablesBuffer.addShape(std::move(obj));
    }
    default:
    {
        throw std::runtime_error("unknown shape");
    }
    }
}

ZviewInfImpl::ReadAck SharedMemoryManager::privReadData() const
{
    ConstMemStream ms(m_data.constData());
    ZviewInfImpl::Command cmd;
    ms >> cmd;
    ZviewInfImpl::ReadAck ack{cmd,{}};
    switch (cmd)
    {
    case ZviewInfImpl::Command::ADD_PCL:
    case ZviewInfImpl::Command::ADD_EDGES:
    case ZviewInfImpl::Command::ADD_MESH:
    {
        
        *reinterpret_cast<int*>(ack.buffer.begin()) = privReadShape(ms, cmd);
        return ack;
    }
    case ZviewInfImpl::Command::GET_HNUM_FROM_HSTR:
    {
        std::string name;
        ms >> name;
        qint64 key = -1;
        for(auto& obj:drawablesBuffer)
        {
            if(obj.second.get()->getName()==name)
            {
                key = obj.first;
                break;
            }
        }
        *reinterpret_cast<int*>(ack.buffer.begin()) = key;
        return ack;
    }
    case ZviewInfImpl::Command::GET_LAST_KEYSTROKE:
    {
        *reinterpret_cast<int*>(ack.buffer.begin()) = emit signal_getLastKeyStroke(true);
        return ack;
    }
    case ZviewInfImpl::Command::UPDATE_PCL:
    {

        qint64 key;
        size_t npoints;
        ms >> key >> npoints;

        const Types::VertData *v = ms.getMemPtr<const Types::VertData *>();
        ack.buffer[0] = drawablesBuffer.updateVertexBuffer(key, v, npoints);
        return ack;
        
    }
    case ZviewInfImpl::Command::REMOVE_SHAPE:
    {
        qint64 key;
        ms >> key;
        ack.buffer[0]  = drawablesBuffer.removeShape(key);
        return ack;
        
    }
    case ZviewInfImpl::Command::SAVE_PLY:
    {
        std::string fn;
        ms >> fn;
        emit signal_savePly(QString::fromStdString(fn));
        ack.buffer[0] = 1U;
        return ack;
        
    }
    case ZviewInfImpl::Command::SET_CAM_LOOKAT:
    {
        QVector3D eye, center, up;
        ms >> eye[0] >> eye[1] >> eye[2] >> center[0] >> center[1] >> center[2] >> up[0] >> up[1] >> up[2];
        emit signal_setCamLookAt(eye, center, up);
        ack.buffer[0] = 1U;
        return ack;

    }
    case ZviewInfImpl::Command::GET_TARGET_XYZ:
    {
        *reinterpret_cast<QVector3D*>(ack.buffer.begin())  = emit signal_getTargetXyz();
        return ack;
    }

    default:
        qWarning() << "unknown command:" << size_t(cmd);
    }
    return ZviewInfImpl::ReadAck{ZviewInfImpl::Command::UNKNOWN, {}};
}

void SharedMemoryManager::proccessSharedMemory()
{
    m_data.lock();
    ZviewInfImpl::ReadAck ack = privReadData();
    m_data.unlock();
    m_ack.lock();
    *static_cast<ZviewInfImpl::ReadAck *>(m_ack.data()) = ack;
    m_ack.unlock();
}

SharedMemoryManager::SharedMemoryManager(QObject *parent) : QObject(parent)
{

    ZviewInfImpl::initSharedMem(&m_data, &m_ack);
    m_th = new CmdQueryThread(this);
    connect(m_th, &CmdQueryThread::dataReady, this, &SharedMemoryManager::proccessSharedMemory);
    connect(m_th, &CmdQueryThread::finished, m_th, &QObject::deleteLater);
    m_th->start();
}

SharedMemoryManager::~SharedMemoryManager()
{
    m_th->stop();
    delete m_th;
}