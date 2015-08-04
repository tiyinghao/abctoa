#include "NodeCache.h"


NodeCache::NodeCache(AtCritSec mycs)
{
	
	lock = mycs;
}

NodeCache::~NodeCache()
{
	AiMsgInfo("\t[Alembic Procedural] Removing %i nodes from the cache", ArnoldNodeCache.size());
	ArnoldNodeCache.clear();

}

//-*************************************************************************
// getCachedNode
// This function return the the mesh node if already in the cache.
// Otherwise, return NULL.
//-*************************************************************************
AtNode* NodeCache::getCachedNode(std::string cacheId)
{
	AiMsgDebug("Searching for %s", cacheId.c_str());
	AtNode* node = NULL;
	//boost::mutex::scoped_lock readLock( lock );
	AiCritSecEnter(&lock);
    std::map<std::string, std::string>::iterator I = ArnoldNodeCache.find(cacheId);
    if (I != ArnoldNodeCache.end())
		node = AiNodeLookUpByName(I->second.c_str());
	AiCritSecLeave(&lock);
	//readLock.unlock();
    return node;
}

//-*************************************************************************
// addNode
// This function adds a node in the cache.
//-*************************************************************************
void NodeCache::addNode(std::string cacheId, AtNode* node)
{
	//boost::mutex::scoped_lock writeLock( lock );
	AiCritSecEnter(&lock);
	AiMsgDebug("Adding node %s (%s) and type %s", AiNodeGetName(node), cacheId.c_str(), AiNodeEntryGetName(AiNodeGetNodeEntry (node)));
	ArnoldNodeCache[cacheId] = std::string(AiNodeGetName(node));
	//writeLock.unlock();
	AiCritSecLeave(&lock);

}


// The node collector is a thread-safe class that accumlate AtNode created in the procedural.
NodeCollector::NodeCollector()
{
	lock.initialize();
}

NodeCollector::~NodeCollector()
{
	AiMsgInfo("Deleting node collector (%i collected nodes)", ArnoldNodeCollector.size());
	ArnoldNodeCollector.clear();
	lock.destroy();
}

//-*************************************************************************
// addNode
// This function adds a node in the node collector.
//-*************************************************************************
void NodeCollector::addNode(AtNode* node)
{
	boost::mutex::scoped_lock writeLock( lock );
	//AiMsgDebug("Adding node %s and type %s", AiNodeGetName(node), AiNodeEntryGetName(AiNodeGetNodeEntry (node)));
	ArnoldNodeCollector.push_back(node);
	writeLock.unlock();

}

size_t NodeCollector::getNumNodes()
{
	size_t size;
	boost::mutex::scoped_lock readLock( lock );
	size = ArnoldNodeCollector.size();
	readLock.unlock();
	return size;
}

AtNode* NodeCollector::getNode(int num)
{
	AtNode* node = NULL;
	boost::mutex::scoped_lock readLock( lock );
	if (num <= ArnoldNodeCollector.size())
		node = ArnoldNodeCollector[num];
	readLock.unlock();
	return node;
}




FileCache::FileCache(AtCritSec mycs)
{
	lock = mycs;
	
}

FileCache::~FileCache()
{
	AiMsgInfo("\t[Alembic Procedural] Removing %i files from the file cache", ArnoldFileCache.size());
	AlembicFileReader.clear();
	openedFiles.clear();
	ArnoldFileCache.clear();

}

//-*************************************************************************
// getCachedNode
// This function return the the mesh node if already in the cache.
// Otherwise, return NULL.
//-*************************************************************************
std::vector<CachedNodeFile> FileCache::getCachedFile(std::string cacheId)
{
	std::vector<CachedNodeFile> createdNodes;
	//boost::mutex::scoped_lock readLock( lock );
	AiCritSecEnter(&lock);
    std::map<std::string, std::vector<CachedNodeFile>>::iterator I = ArnoldFileCache.find(cacheId);
    if (I != ArnoldFileCache.end())
		createdNodes = I->second;

	//readLock.unlock();
	AiCritSecLeave(&lock);
    return createdNodes;
}

const size_t FileCache::hash( std::string const& s )
{
    size_t result = 2166136261U ;
    std::string::const_iterator end = s.end() ;
    for ( std::string::const_iterator iter = s.begin() ;
            iter != end ;
            ++ iter )
    {
        result = 127 * result
                + static_cast< unsigned char >( *iter ) ;
    }
    return result ;
 }

std::string FileCache::getHash(std::string fileName,     
							   std::map<std::string, AtNode*> shaders,
							   std::map<std::string, AtNode*> displacements,
							   std::vector<std::string> attributes
							   )
{

	std::ostringstream shaderBuff;
	for(std::map<std::string, AtNode*>::iterator it = shaders.begin(); it != shaders.end(); ++it) 
	{
		shaderBuff << it->first;
	}

	std::ostringstream displaceBuff;
	for(std::map<std::string, AtNode*>::iterator it = displacements.begin(); it != displacements.end(); ++it) 
	{
		displaceBuff << it->first;
	}

	std::ostringstream attributesBuff;
	for(std::vector<std::string>::iterator it = attributes.begin(); it != attributes.end(); ++it) 
	{
		attributesBuff << *it;
	}

	std::ostringstream buffer;
	std::string cacheId;

	
    buffer << hash(fileName) << "@" << hash(shaderBuff.str()) << "@" << hash(displaceBuff.str()) << "@" << hash(attributesBuff.str());

    cacheId = buffer.str();
	return cacheId;

}

//-*************************************************************************
// addNode
// This function adds a node in the cache.
//-*************************************************************************
void FileCache::addCache(std::string cacheId, NodeCollector* createdNodes)
{
	//boost::mutex::scoped_lock writeLock( lock );
	AiCritSecEnter(&lock);

	for(int i = 0; i <  createdNodes->getNumNodes(); i++)
	{
		
		AtNode* node = createdNodes->getNode(i);

		if(AiNodeEntryGetType(AiNodeGetNodeEntry(node)) == AI_NODE_SHAPE)
		{
			if(AiNodeGetByte(node, "visibility") != 0)
			{
				CachedNodeFile cachedNode;
				cachedNode.node = node;
				cachedNode.matrix = AiNodeGetArray(node, "matrix");

				//AiMsgInfo("ADDCACHE Getting obj %i %s and type %s", i, AiNodeGetName(createdNodes->getNode(i)), AiNodeEntryGetName(AiNodeGetNodeEntry (createdNodes->getNode(i))));
				//newCreatedNodes->addNode(createdNodes->getNode(i));
				ArnoldFileCache[cacheId].push_back(cachedNode);
			}

		}
		else if (AiNodeEntryGetType(AiNodeGetNodeEntry(node)) == AI_NODE_LIGHT)
		{
			CachedNodeFile cachedNode;
			cachedNode.node = node;
			cachedNode.matrix = AiNodeGetArray(node, "matrix");
			ArnoldFileCache[cacheId].push_back(cachedNode);
		}
	}

	//ArnoldFileCache[cacheId] = newCreatedNodes;
	//writeLock.unlock();
	AiCritSecLeave(&lock);

}

void FileCache::addToOpenedFiles(std::string filename)
{
	//boost::mutex::scoped_lock writeLock( lock );
	AiCritSecEnter(&lock);
	if(std::find(openedFiles.begin(), openedFiles.end(), filename) == openedFiles.end())
		openedFiles.push_back(filename);

	AiCritSecLeave(&lock);
	//writeLock.unlock();
}


void FileCache::addReader(std::string filename)
{
	//boost::mutex::scoped_lock writeLock( lock );
	AiCritSecEnter(&lock);
	Alembic::AbcCoreFactory::IFactory factory;
	factory.setOgawaNumStreams(8);
    IArchive archive = factory.getArchive(filename);
    if (!archive.valid())
    {
        AiMsgError ( "Cannot read file %s", filename.c_str());
    }
    else
    {
        AiMsgDebug ( "reading file %s", filename.c_str());
    }	

	AlembicFileReader[filename] = archive;

	//writeLock.unlock();
	AiCritSecLeave(&lock);
}

IArchive FileCache::getReader(std::string filename)
{

	//addToOpenedFiles(filename);
	IArchive root;
	//boost::mutex::scoped_lock writeLock( lock );
	AiCritSecEnter(&lock);
	root = AlembicFileReader[filename];
	
	//writeLock.unlock();
	AiCritSecLeave(&lock);
	return root;

}

void FileCache::removeFromOpenedFiles(std::string filename)
{
	//boost::mutex::scoped_lock writeLock( lock );
	AiCritSecEnter(&lock);
	std::vector<std::string>::iterator it = std::find(openedFiles.begin(), openedFiles.end(), filename);
	if(it != openedFiles.end())
		openedFiles.erase(it);

	//writeLock.unlock();
	AiCritSecLeave(&lock);
}

bool FileCache::isFileOpened(std::string filename)
{
	//boost::mutex::scoped_lock readLock( lock );
	AiCritSecEnter(&lock);
	std::vector<std::string>::iterator it = std::find(openedFiles.begin(), openedFiles.end(), filename);
	//readLock.unlock();
	AiCritSecLeave(&lock);

	if(it != openedFiles.end())
		return true;

	else
		return false;
	
}