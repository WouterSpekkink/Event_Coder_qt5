(TeX-add-style-hook
 "Event_Coder_Manual"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-package-options
                     '(("fontenc" "T1")))
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
    "framed"
    "fontenc")
   (LaTeX-add-labels
    "chap:introduction"
    "sec:eventcoderprogram"
    "sec:partofbiggerprogram"
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
    "fig:flaggedincident"
    "sec:memosincidents"
    "sec:exportingdata"
    "fig:exportdialog"
    "sec:logfilesandexiting"
    "chap:usingtheprogram2"
    "sec:introductiontocoding"
    "sec:attributemodule"
    "fig:usesofattributes"
    "sec:creatingnewattributes"
    "fig:attributemodule"
    "fig:attributedialog"
    "fig:attributepool"
    "sec:filteringattributes"
    "fig:filteringattributes"
    "sec:editingattributes"
    "sec:removingattributes"
    "sec:incidentattributecategories"
    "fig:categorydialog"
    "fig:attributedialogwithcategories"
    "sec:assigningunassigningattributes"
    "fig:assignedattributes"
    "sec:relationshipmodule"
    "fig:relationshipmodule"
    "sec:creatingnewrelationships1"
    "fig:relationshipdialog"
    "sec:entities"
    "fig:entitydialog"
    "sec:relationshiptypes"
    "fig:relationshiptypedialog"
    "sec:creatingnewrelationships2"
    "fig:relationshipdialog2"
    "sec:editingrelationships"
    "sec:removingrelationships"
    "chap:whatisnext"
    "chap:contactdetails"))
 :latex)

