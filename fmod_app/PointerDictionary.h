#ifndef POINTERDICTIONARY_H
#define POINTERDICTIONARY_H

#include <juce/juce.h>

/** A class to store pointers with an associated string key. */
template<class ObjectType>
class PointerDictionary
{
private:
	StringArray ids;
	Array<ObjectType*> objects;
	
public:
	
	/** Add a named item to the dictionary.
	 
	 @param name	The name of the object to add.
	 @param obj		The pointer to associate with the name. 
	 @return		0 if the name was added to the dictionary or the old object
					if this name was already in the dictionary. */
	ObjectType* add(String const& name, ObjectType* obj)
	{
		int index = ids.indexOf(name);
		
		if(index >= 0)
		{
			ObjectType* oldObj = objects[index];
			objects.set(index, obj);
			return oldObj;
		}
		else
		{
			ids.add(name);
			objects.add(obj);
			return 0;
		}
	}
	
	/** Returns a named object from the dictionary. */
	ObjectType* get(String const& name)
	{
		int index = ids.indexOf(name);
		return objects[index];
	}
	
	/** Removes and returns a named object from the dictionary. */
	ObjectType* remove(String const& name)
	{
		int index = ids.indexOf(name);
		
		if(index >= 0)
		{
			ObjectType* object = objects[index];
			ids.remove(index);
			objects.remove(index);
			return object;
		}
		else
		{
			return 0;
		}
	}
	
	/** Clears the dictionary. */
	void clear()
	{
		ids.clear();
		objects.clear();
	}
    
    void clearAndDelete()
	{
		ids.clear();
        
        for (int i = 0; i < objects.size(); i++)
            delete objects[i];
        
		objects.clear();
	}

	
};

#endif // POINTERDICTIONARY_H