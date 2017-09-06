(TeX-add-style-hook
 "Event_Coder_Manual"
 (lambda ()
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "path")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "url")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "nolinkurl")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "hyperbaseurl")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "hyperimage")
   (add-to-list 'LaTeX-verbatim-macros-with-braces-local "hyperref")
   (add-to-list 'LaTeX-verbatim-macros-with-delims-local "path")
   (TeX-run-style-hooks
    "latex2e"
    "memoir"
    "memoir10"
    "xcolor"
    "hyperref"
    "graphicx"
    "enumitem"
    "verbatim"
    "framed")
   (LaTeX-add-labels
    "chap:introduction"
    "sec:disclaimers"
    "chap:preparations"
    "sec:datasets"
    "sec:othernotesdatasets"
    "chap:usingtheprogram"
    "sec:loadingnewdataset"
    "fig:importoptions"
    "sec:importerrors"
    "fig:importerror"
    "sec:savingloadingdata"
    "fig:saveload"
    "sec:importingcodes"
    "fig:importcodesfig"
    "fig:importingcodesdiagram"
    "sec:problemsimportingcodes"
    "fig:overwritingcodes"
    "sec:navigatingdata"
    "fig:incidentsoverview"
    "sec:markingincidents"
    "sec:attributes"
    "sec:relationships"
    "chap:contactdetails"))
 :latex)

