bin_PROGRAMS = encdetect
encdetect_SOURCES = src/main.cpp
encdetect_CXXFLAGS = \
                     $(LCF_CFLAGS) \
                     $(ICU_CFLAGS) \
                     $(ENCA_CFLAGS) \
                     $(GUESS_CFLAGS)
encdetect_LDADD = \
                  $(LCF_LIBS) \
                  $(ICU_LIBS) \
                  $(ENCA_LIBS) \
                  $(GUESS_LIBS)

EXTRA_DIST = README.md AUTHORS
