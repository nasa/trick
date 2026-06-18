#=============================================================================
# Allow user to specify their own package locations.
#   - File is skipped if not present
#=============================================================================

#=============================================================================
# Set up the include paths
#=============================================================================

TRICK_CFLAGS	:= -I${PWD} -I.. 
TRICK_CXXFLAGS	:= -I${PWD} -I.. 


#=============================================================================
# Add target to clean the documents directories of TeX related files.
#=============================================================================
clean_docs:
	@rm -f *_log *execution *_hs *_summary
	@cd docs; rm -f *.aux *.bbl *.blg *.lof *.log *.lot *.toc 
	@cd docs/tex; rm -f *.aux *.log
	@echo Cleaned documents directory.

spotless: clean_docs

