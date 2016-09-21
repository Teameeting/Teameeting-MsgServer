define BUILD_LIBRARY
$(if $(wildcard $@),@$(RM) $@)
$(if $(wildcard ar.mac),@$(RM) ar.mac)
$(if $(filter lib%.a, $^),
@$(ECHO) CREATE $@ > ar.mac
@$(ECHO) SAVE >> ar.mac
@$(ECHO) END >> ar.mac
@$(AR) -M < ar.mac)
$(if $(filter %.o,$^),@$(AR) -q $@ $(filter %.o, $^))
$(if $(filter %.a, $^),
@$(ECHO) OPEN $@ > ar.mac
$(foreach LIB, $(filter lib%.a, $^),
@echo ADDLIB $(LIB) >> ar.mac)
@$(ECHO) SAVE >> ar.mac
@$(ECHO) END >> ar.mac
@$(AR) -M < ar.mac)
endef
