// ----------------------------------------------------------------------------
//  Description      : Garbage collection
// ----------------------------------------------------------------------------
//  Remarks          : "Normal" reference management works with exactly one
//                     reference manager per type which can be implicitly
//                     identified, whereas dynamic management allows
//                     more than one manager.
//
// ----------------------------------------------------------------------------
//  (c) Copyright 2000 by iXiONmedia, all rights reserved.
// ----------------------------------------------------------------------------




#ifndef IXLIB_GARBAGE
#define IXLIB_GARBAGE




#include <memory>
#include <hash_map>
#include <ixlib_base.hh>




namespace ixion {
  template<class T,class Allocator = allocator<T> >
  class reference_manager;




  template<class T>
  class ref {
      static reference_manager<T> 	Manager;
      T					*Instance;
      
    public:
      ref(ref const &src)
        : Instance(src.Instance) {
	Manager.addReference(Instance);
	}
      ref(T *instance = NULL)
        : Instance(instance) {
	Manager.addReference(Instance);
	}
      ~ref() {
        Manager.removeReference(Instance);
	}
      
      // smart pointer nitty-gritty
      T &operator*() const {
        return Instance;
	}
      T *operator->() const {
        return Instance;
	}
      T *operator+(TIndex index) const {
        return Instance + index;
	}
      T &operator[](TIndex index) const {
        return Instance[index];
	}
      
      // methods
      T *get() const {
        return Instance;
	}
      void release() {
        T *oldinst = Instance;
	Manager.removeReference(Instance);
	Instance = NULL;
	}
      void set(T *instance) {
	Manager.removeReference(Instance);
	Instance = instance;
	Manager.addReference(Instance);
        }
    };
    
    
  
  
  #define IXLIB_GARBAGE_MAKE_MANAGER(TYPE) \
    ixion::reference_manager<TYPE> ixion::ref<TYPE>::Manager;




  template<class T>
  class dynamic_ref {
      reference_manager<T> 	&Manager;
      T				*Instance;
      
    public:
      dynamic_ref(dynamic_ref const &src)
        : Manager(src.Manager),Instance(src.Instance) {
	Manager.addReference(Instance);
	}
      dynamic_ref(reference_manager<T> &mgr)
        : Manager(mgr),Instance(NULL) {
	}
      dynamic_ref(reference_manager<T> &mgr,T *instance)
        : Manager(mgr),Instance(instance) {
	Manager.addReference(Instance);
	}
      ~dynamic_ref() {
        Manager.removeReference(Instance);
	}
      
      // smart pointer nitty-gritty
      T &operator*() const {
        return Instance;
	}
      T *operator->() const {
        return Instance;
	}
      T *operator+(TIndex index) const {
        return Instance + index;
	}
      T &operator[](TIndex index) const {
        return Instance[index];
	}
      
      // methods
      T *get() const {
        return Instance;
	}
      T *release() {
        T *oldinst = Instance;
	Manager.removeReference(Instance);
	Instance = NULL;
	return oldinst;
	}
      T *set(T *instance) {
        T *oldinst = Instance;
	Manager.removeReference(Instance);
	Instance = instance;
	Manager.addReference(Instance);
	return oldinst;
        }
    };
  
  
  
  
  template<class T,class Allocator>
  class reference_manager {
    protected:
      struct instance_data {
        TSize ReferenceCount;
	
	instance_data() 
	  : ReferenceCount(0) {
	  }
	};
      
      class pointer_hash {
        public:
          unsigned operator()(T const *ptr) const {
	    unsigned u = reinterpret_cast<unsigned>(ptr);
	    return (u ^ (u >> 8) ^ (u >> 16) ^ (u >> 24)) & 0x1ff;
            }
	};
      hash_map<T const *,instance_data,pointer_hash>	Instances;
      Allocator						Alloc;
      
    public:
      reference_manager(Allocator const &alloc = Allocator())
        : Alloc(alloc) {
	}
    
    protected:
      void addReference(T const *instance) {
        if (!instance) return;
        Instances[instance].ReferenceCount++;
        }
      void removeReference(T const *instance) {
        if (!instance) return;
        TSize refc = --Instances[instance].ReferenceCount;
	if (refc == 0) {
	  Instances.erase(instance);
	  Alloc.deallocate(const_cast<T *>(instance),1);
	  }
        }
      
      friend class dynamic_ref<T>;
      friend class ref<T>;
    };
  }




#endif