/************************************************************************
	filename: 	CEGUIImagesetManager.cpp
	created:	21/2/2004
	author:		Paul D Turner
	
	purpose:	Implements the ImagesetManager class
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#include "CEGUIImagesetManager.h"
#include "CEGUIExceptions.h"
#include "CEGUIImageset.h"
#include "CEGUISystem.h"
#include "CEGUITexture.h"
#include <iostream>
// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Static Data Definitions
*************************************************************************/
// singleton instance pointer
template<> ImagesetManager* Singleton<ImagesetManager>::ms_Singleton	= NULL;


/*************************************************************************
	Constructor
*************************************************************************/
ImagesetManager::ImagesetManager(void)
{
}


/*************************************************************************
	Destructor
*************************************************************************/
ImagesetManager::~ImagesetManager(void)
{
	destroyAllImagesets();
}


/*************************************************************************
	Create an empty Imageset that has the given name and uses the
	given Texture
*************************************************************************/
Imageset* ImagesetManager::createImageset(const String& name, Texture* texture, bool d_haveBorder)
{
	if (isImagesetPresent(name))
	{
		throw	AlreadyExistsException("ImagesetManager::createImageset - An Imageset object named '" + name + "' already exists.");
	}

	Imageset* temp = new Imageset(name, texture, d_haveBorder);
	d_imagesets[name] = temp;

	return temp;
}


/*************************************************************************
	Create an Imageset object from the specified file
*************************************************************************/
Imageset* ImagesetManager::createImageset(const String& filename, const String& resourceGroup, bool d_haveBorder)
{
	Imageset* temp = new Imageset(filename, resourceGroup, d_haveBorder);

	String	name = temp->getName();

	if (isImagesetPresent(name))
	{
		delete temp;

		throw	AlreadyExistsException("ImagesetManager::createImageset - An Imageset object named '" + name + "' already exists.");
	}

	d_imagesets[name] = temp;

	return temp;
}


/*************************************************************************
    Create an Imageset object from the specified image file.
*************************************************************************/
Imageset* ImagesetManager::createImagesetFromImageFile(const String& name, const String& filename, const String& resourceGroup, bool d_haveBorder)
{
    if (isImagesetPresent(name))
    {
        throw	AlreadyExistsException("ImagesetManager::createImageset - An Imageset object named '" + name + "' already exists.");
    }

    Imageset* temp = new Imageset(name,  filename, resourceGroup, d_haveBorder);
    d_imagesets[name] = temp;

    return temp;
}

Imageset* 
ImagesetManager::createImagesetFromMemory( const String& name , void* pBuffer, const String& resourceGroup , bool d_haveBorder )
{
	if (isImagesetPresent(name))
    {
        throw	AlreadyExistsException("ImagesetManager::createImageset - An Imageset object named '" + name + "' already exists.");
    }
	Texture* pTexture = System::getSingleton ().getRenderer( )->createTextureFromMemory( pBuffer );

	if( pTexture == 0 )
	{
		return NULL;
	}
	Imageset* temp = new Imageset( name, pTexture, d_haveBorder );

	
	unsigned int uWidth = pTexture->getWidth( );
	unsigned int uHeight = pTexture->getHeight( );
	temp->defineImage(
        "full_image",
        Rect(0, 0, uWidth, uHeight ),
        Point(0, 0)
      );
	d_imagesets[name] = temp;

	return temp;
}


/*************************************************************************
	Destroys the Imageset with the specified name
*************************************************************************/
void ImagesetManager::destroyImageset(const String& name)
{
	ImagesetRegistry::iterator	pos = d_imagesets.find(name);

	if (pos != d_imagesets.end())
	{
		String tmpName(name);

		delete pos->second;
		d_imagesets.erase(pos);

	}

}


/*************************************************************************
	Destroys the given Imageset object
*************************************************************************/
void ImagesetManager::destroyImageset(Imageset* imageset)
{
	if (imageset != NULL)
	{
		destroyImageset(imageset->getName());
	}

}


/*************************************************************************
	Destroy all Imageset objects
*************************************************************************/
void ImagesetManager::destroyAllImagesets(void)
{
	while (!d_imagesets.empty())
	{
		destroyImageset(d_imagesets.begin()->first);
	}
}

/*************************************************************************
	Returns a pointer to the Imageset object with the specified name
*************************************************************************/
Imageset* ImagesetManager::getImageset(const String& name) const
{
	ImagesetRegistry::const_iterator	pos = d_imagesets.find(name);

	if (pos == d_imagesets.end())
	{
		throw UnknownObjectException("ImagesetManager::getImageset - No Imageset named '" + name + "' is present in the system.");
	}

	return pos->second;
}


/*************************************************************************
	Notify the ImagesetManager of the current (usually new) display
	resolution.
*************************************************************************/
void ImagesetManager::notifyScreenResolution(const Size& size)
{
	// notify all attached Imageset objects of the change in resolution
	ImagesetRegistry::iterator pos = d_imagesets.begin(), end = d_imagesets.end();

	for (; pos != end; ++pos)
	{
		pos->second->notifyScreenResolution(size);
	}

}


ImagesetManager& ImagesetManager::getSingleton(void)
{
	return Singleton<ImagesetManager>::getSingleton();
}


ImagesetManager* ImagesetManager::getSingletonPtr(void)
{
	return Singleton<ImagesetManager>::getSingletonPtr();
}


/*************************************************************************
	Return a ImagesetManager::ImagesetIterator object to iterate over
	the available Imageset objects.
*************************************************************************/
ImagesetManager::ImagesetIterator ImagesetManager::getIterator(void) const
{
	return ImagesetIterator(d_imagesets.begin(), d_imagesets.end());
}


void ImagesetManager::writeImagesetToStream(const String& imageset, OutStream& out_stream) const
{
    const Imageset* iset = getImageset(imageset);

    // output xml header
    out_stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;

    // output imageset data
    iset->writeXMLToStream(out_stream);
}


} // End of  CEGUI namespace section
