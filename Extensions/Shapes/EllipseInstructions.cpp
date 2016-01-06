#if defined(GD_IDE_ONLY)
#include "EllipseInstructions.h"

#include "GDCore/Tools/Localization.h"

void DeclareEllipseObjectInstruction(gd::ObjectMetadata & obj)
{
    obj.AddAction("FillColor",
                   _("Modify the fill color"),
                   _("Modify the fill color of an ellipse object."),
                   _("Do _PARAM1__PARAM2_ to the text of _PARAM0_"),
                   _(""),
                   "res/actions/text24.png",
                   "res/actions/text.png")

        .AddParameter("object", _("Object"), "Ellipse")
        .AddParameter("color", _("Background color"))
        .SetFunctionName("SetFillColor").SetIncludeFile("Shapes/RuntimeEllipseObject.h");
}

#endif
