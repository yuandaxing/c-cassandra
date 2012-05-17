SUBDIRS=splay_rb_tree debug serialize utils/ config/ seda/  mem db 
.PHONY:$(SUBDIRS)


all:$(SUBDIRS)

depend dep:
	@for dir in $(SUBDIRS);do $(MAKE) -C $$dir .depend; done

$(SUBDIRS):
	$(MAKE) -C $@ all

	  
clean:
	@for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done
