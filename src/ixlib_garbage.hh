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
#include <ixlib_exgen.hh>
#include <ixlib_base.hh>




namespace ixion {
  template<class T>
  class delete_deallocator {
    public:
      void operator()(T const *instance) {
        delete instance;
	}
    };
  template<class T>
  class delete_array_deallocator {
    public:
      void operator()(T const *instance) {
        delete[] instance;
	}
    };

  template<class T,class Deallocator = delete_deallocator<T> >
  class reference_manager;

  template<class T>
  class ref_base {
    protected:
      T					*Instance;
    
    public:
      ref_base(T *instance = NULL)
        : Instance(instance) {
	}
      ref_base(ref_base const &src) 
        : Instance(src.Instance) {
	}

      // comparison
      bool operator==(ref_base const &op2) const {
        return Instance == op2.Instance;
        }
      
      // smart pointer nitty-gritty
      T &operator*() const {
        return *Instance;
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
    };




  template<class T>
  class no_free_ref;
  
  
  
  
  template<class T>
  class ref : public ref_base<T>{
    protected:
      static reference_manager<T> 	Manager;
      
    public:
      ref(ref const &src)
        : ref_base<T>(src) {
	Manager.addReference(Instance);
	}
      ref(T *instance = NULL)
        : ref_base<T>(instance) {
	Manager.addReference(instance);
	}
      ~ref() {
        Manager.freeReference(Instance);
	}
	
      ref &operator=(ref const &src) {
        set(src.get());
        return *this;
        }
      ref &operator=(T *ptr) {
        set(ptr);
        return *this;
        }
      
      // methods
      void release() {
        T *oldinst = Instance;
	Manager.freeReference(Instance);
	Instance = NULL;
	}
      void set(T *instance) {
	Manager.freeReference(Instance);
	Instance = instance;
	Manager.addReference(Instance);
        }

      friend class no_free_ref<T>;
    };




  template<class T>
  class no_free_ref : public ref_base<T>{
    public:
      no_free_ref(no_free_ref const &src)
        : ref_base<T>(src) {
	ref<T>::Manager.addReference(Instance);
	}
      no_free_ref(T *instance = NULL)
        : ref_base<T>(instance) {
	ref<T>::Manager.addReference(instance);
	}
      ~no_free_ref() {
        ref<T>::Manager.removeReference(Instance);
	}
	
      // assignment
      no_free_ref &operator=(no_free_ref const &src) {
        set(src.get());
        return *this;
        }
      no_free_ref &operator=(T *ptr) {
        set(ptr);
        return *this;
        }
      
      // methods
      void release() {
        T *oldinst = Instance;
	ref<T>::Manager.removeReference(Instance);
	Instance = NULL;
	}
      void set(T *instance) {
	ref<T>::Manager.removeReference(Instance);
	Instance = instance;
	ref<T>::Manager.addReference(Instance);
        }
    };




  template<class T>
  class dynamic_ref : public ref_base<T> {
    protected:
      reference_manager<T> 	&Manager;
      
    public:
      dynamic_ref(dynamic_ref const &src)
        : ref_base<T>(src),Manager(src.Manager) {
	Manager.addReference(Instance);
	}
      dynamic_ref(reference_manager<T> &mgr,T *instance = NULL)
        : ref_base<T>(instance),Manager(mgr) {
	Manager.addReference(Instance);
	}
      ~dynamic_ref() {
        Manager.freeReference(Instance);
	}
      
      // assignment
      dynamic_ref &operator=(dynamic_ref const &src) {
        set(src.get());
        return *this;
        }
      dynamic_ref &operator=(T *ptr) {
        set(ptr);
        return *this;
        }

      // methods
      T *release() {
        T *oldinst = Instance;
	Manager.freeReference(Instance);
	Instance = NULL;
	return oldinst;
	}
      T *set(T *instance) {
        T *oldinst = Instance;
	Manager.freeReference(Instance);
	Instance = instance;
	Manager.addReference(Instance);
	return oldinst;
        }
    };
  
  
  
  
  template<class T>
  class no_free_dynamic_ref : public ref_base<T> {
    protected:
      reference_manager<T> 	&Manager;
      
    public:
      no_free_dynamic_ref(no_free_dynamic_ref const &src)
        : ref_base<T>(src),Manager(src.Manager) {
	Manager.addReference(Instance);
	}
      no_free_dynamic_ref(reference_manager<T> &mgr,T *instance = NULL)
        : ref_base<T>(instance),Manager(mgr) {
	Manager.addReference(Instance);
	}
      ~no_free_dynamic_ref() {
        Manager.removeReference(Instance);
	}
      
      // assignment
      no_free_dynamic_ref &operator=(no_free_dynamic_ref const &src) {
        set(src.get());
        return *this;
        }
      no_free_dynamic_ref &operator=(T *ptr) {
        set(ptr);
        return *this;
        }

      // methods
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
  
  
  
  
  template<class T,class Deallocator>
  class reference_manager {
    protected:
    
      struct instance_data {
	T const		*Instance;
        TSize 		ReferenceCount;
	instance_data	*Next,*Previous;
	};
      
      class pointer_hash {
        public:
	};

      typedef unsigned hash_value;
      static hash_value const HASH_MAX = 0x3ff;
      
      instance_data					*Instances[HASH_MAX+1];
      Deallocator					Dealloc;
      
    public:
      reference_manager(Deallocator const &dealloc = Deallocator())
        : Dealloc(dealloc) {
	for (hash_value hv = 0;hv <= HASH_MAX;hv++)
	  Instances[hv] = NULL;
	}
    
    protected:
      void addReference(T const *instance) {
        if (!instance) return;
	instance_data *data = getHashEntry(instance);
        data->ReferenceCount++;
        }
      void freeReference(T const *instance) {
        if (!instance) return;
	instance_data *data = getHashEntry(instance);
	if (--data->ReferenceCount == 0) {
	  removeHashEntry(data);
	  Dealloc(instance);
	  }
        }
      void removeReference(T const *instance) {
        if (!instance) return;
	instance_data *data = getHashEntry(instance);
	if (--data->ReferenceCount == 0) {
	  removeHashEntry(data);
	  }
        }
	
    private:
      hash_value hash(T const *ptr) const {
        unsigned u = reinterpret_cast<unsigned>(ptr);
        return (u ^ (u >> 8) ^ (u >> 16) ^ (u >> 24)) & HASH_MAX;
        }
      instance_data *getHashEntry(T const *instance) {
        instance_data *data = Instances[hash(instance)];
	while (data) {
	  if (data->Instance == instance) return data;
	  data = data->Next;
	  }
	
	// not found, add new at front
        instance_data *link = Instances[hash(instance)];
	data = new instance_data;
	EX_MEMCHECK(data)
	
	data->Instance = instance;
	data->ReferenceCount = 0;
	data->Previous = NULL;
	data->Next = link;
	if (link) link->Previous = data;
	Instances[hash(instance)] = data;
	return data;
        }
      void removeHashEntry(instance_data *data) {
        instance_data *prev = data->Previous;
        if (prev) {
	  prev->Next = data->Next;
	  if (data->Next) data->Next->Previous = prev;
	  delete data;
	  }
	else {
	  Instances[hash(data->Instance)] = data->Next;
	  if (data->Next) data->Next->Previous = NULL;
	  delete data;
	  }
        }
	
      friend class ref<T>;
      friend class no_free_ref<T>;
      friend class dynamic_ref<T>;
      friend class no_free_dynamic_ref<T>;
    };




  #define IXLIB_GARBAGE_DECLARE_MANAGER(TYPE) \
    ixion::reference_manager<TYPE> ixion::ref<TYPE>::Manager;
  }




#endif