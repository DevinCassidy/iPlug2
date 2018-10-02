#pragma once

#include "IControl.h"

/** A base control for a pop-up menu/drop-down list that stays within the bounds of the IGraphics context.
 * For replacing generic menus this can be added with IGraphics::AttachPopupMenu().
 * If used in the main IControl stack, you probably want it to be the very last control that is added, so that it gets drawn on top.  */
class IPopupMenuControl : public IControl
{
public:
  /** An enumerated list, that is used to determine the state of the menu, mainly for animations*/
  enum EPopupState
  {
    kCollapsed = 0,
    kExpanding,
    kExpanded,
    kCollapsing,
    kFlickering, // on click
    kSubMenuAppearing, // when a submenu appears, only that menu is faded in
    kIdling,
  };
  
  /** Create a new IPopupMenuControl
  * @param dlg The editor delegate that this control is attached to
  * @param paramIdx Whether this control should be linked to a parameter
  * @param text An IText specifying properties of the menu text
  * @param collapsedBounds If this control, when collapsed should occupy an area of the graphics context, specify this, otherwise the collapsed area is empty
  * @param expandedBounds If you want to explicitly specify the size of the expanded pop-up, you can specify an area here */
  IPopupMenuControl(IGEditorDelegate& dlg, int paramIdx = kNoParameter, IText text = IText(16), IRECT collapsedBounds = IRECT(), IRECT expandedBounds = IRECT());
  virtual ~IPopupMenuControl();
  
  //IControl
  void Draw(IGraphics& g) override;
  void OnMouseDown(float x, float y, const IMouseMod& mod) override;
  void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod) override;
  void OnMouseOver(float x, float y, const IMouseMod& mod) override;
  void OnMouseOut() override;
  void OnEndAnimation() override;
  
  //IPopupMenuControl
  
  /** Called as the user moves the mouse around, in order to work out which menu panel should be on the screen, and create new ones as necessary (for submenus)
   @param x Mouse X position
   @param y Mouse Y position */
  void CalculateMenuPanels(float x, float y);
  
  /** Override this method to change the background of the pop-up menu panel */
  virtual void DrawPanelBackground(IGraphics& g, const IRECT& bounds, IBlend* blend);
  /** Override this method to change the shadow of the pop-up menu panel */
  virtual void DrawPanelShadow(IGraphics& g, const IRECT& bounds, IBlend* blend);
  /** Override this method to change the way a cell's background is drawn */
  virtual void DrawCellBackground(IGraphics& g, const IRECT& bounds, const IPopupMenu::Item* pItem, bool sel, IBlend* blend);
  /** Override this method to change the way a cell's text is drawn */
  virtual void DrawCellText(IGraphics& g, const IRECT& bounds, const IPopupMenu::Item* pItem, bool sel, IBlend* blend);
  /** Override this method to change the way a checked cell's "tick" is drawn */
  virtual void DrawTick(IGraphics& g, const IRECT& bounds, const IPopupMenu::Item* pItem, bool sel, IBlend* blend);
  /** Override this method to change the way a submenu cell's arrow is drawn */
  virtual void DrawSubMenuArrow(IGraphics& g, const IRECT& bounds, const IPopupMenu::Item* pItem, bool sel, IBlend* blend);
  /** Override this method to change the way a scroll up cell's arrow is drawn */
  virtual void DrawUpArrow(IGraphics& g, const IRECT& bounds, bool sel, IBlend* blend);
  /** Override this method to change the way a scroll Down cell's arrow is drawn */
  virtual void DrawDownArrow(IGraphics& g, const IRECT& bounds, bool sel, IBlend* blend);
  /** Override this method to change the way a cell separator is drawn  */
  virtual void DrawSeparator(IGraphics& g, const IRECT& bounds, IBlend* blend);
  
  /** Call this to create a pop-up menu
   @param menu Reference to a menu from which to populate this user interface control. NOTE: this object should not be a temporary, otherwise when the menu returns asynchronously, it may not exist.
   @param pCaller The IControl that called this method, and will receive the call back after menu selection
   @return the menu */
  IPopupMenu* CreatePopupMenu(IPopupMenu& menu, const IRECT& bounds, IControl* pCaller);
  
  /** @return \true if the pop-up is fully expanded */
  bool GetExpanded() const { return mState == kExpanded; }
  
  /**@return EPopupState indicating the state of the pop-up */
  EPopupState GetState() const { return mState; }
private:
  /** Get an IRECT represents the maximum dimensions of the longest text item in the menu */
  IRECT GetLargestCellRectForMenu(IPopupMenu& menu, float x, float y) const;
  
  /** This method is called to expand the modal pop-up menu. It calculates the dimensions and wrapping, to keep the cells within the graphics context. It handles the dirtying of the graphics context, and modification of graphics behaviours such as tooltips and mouse cursor */
  void Expand(float x, float y);
  
  /** This method is called to collapse the modal pop-up menu and make it invisible. It handles the dirtying of the graphics context, and modification of graphics behaviours such as tooltips and mouse cursor */
  virtual void CollapseEverything();
  
private:
  
  /** MenuPanel is used to manage the rectangle of a single menu, and the rectangles for the cells for each menu item */
  class MenuPanel
  {
  public:
    MenuPanel(IPopupMenuControl& owner, const IRECT& contextBounds, IPopupMenu& menu, float x, float y, int parentIdx);
    ~MenuPanel();
    
    /** Get's the width of a cell */
    float CellWidth() const { return mSingleCellBounds.W(); }
    
    /** Get's the height of a cell */
    float CellHeight() const { return mSingleCellBounds.H(); }
    
    void ScrollUp() { mScrollItemOffset--; mScrollItemOffset = Clip(mScrollItemOffset, 0, mCellBounds.GetSize()-1); }
    
    void ScrollDown() { mScrollItemOffset++; mScrollItemOffset = Clip(mScrollItemOffset, 0, mMenu.NItems()-mCellBounds.GetSize()); }

    /** Checks if any of the expanded cells for this panel contain a x, y coordinate, and if so returns an IRECT pointer to the cell bounds
     * @param x X position to test
     * @param y Y position to test
     * @return Pointer to the cell bounds IRECT, or nullptr if nothing got hit */
    IRECT* HitTestCells(float x, float y) const;
    
   public:
    IPopupMenu& mMenu; // The IPopupMenu that this MenuPanel is displaying
    WDL_PtrList<IRECT> mCellBounds; // The size of this array will always correspond to the number of items in the top level of the menu
    
    IRECT mRECT; // The drawing bounds for this panel
    IRECT mTargetRECT; // The mouse target bounds for this panel
    int mScrollMaxRows = 0; // 0 when no scroll
    bool mShouldDraw = true; // boolean determining whether this panel should be drawn
    IBlend mBlend = { kBlendNone, 0.f }; // blend for sub panels appearing
    
    IRECT mSingleCellBounds; // The dimensions of the largest cell for the menu
    IRECT* mHighlightedCell = nullptr; // A pointer to one of the IRECTs in mCellBounds, if one should be highlighted
    IRECT* mClickedCell = nullptr; // A pointer to one of the IRECTs in mCellBounds, if one has been clicked
    int mParentIdx = 0; // An index into the IPopupMenuControl::mMenuPanels lists, representing the parent menu panel
    bool mScroller = false;
    int mScrollItemOffset = 0;
  };
  
  WDL_PtrList<MenuPanel> mMenuPanels; // An array of ptrs to MenuPanel objects for every panel, expands as sub menus are revealed, contents deleted when the menu is dismissed
  MenuPanel* mActiveMenuPanel = nullptr; // A pointer to the active MenuPanel within the mMenuPanels array
  MenuPanel* mAppearingMenuPanel = nullptr; // A pointer to a MenuPanel that's in the process of fading in
  EPopupState mState = kCollapsed; // The state of the pop-up, mainly used for animation
  IRECT* mMouseCellBounds = nullptr;
  IRECT* mPrevMouseCellBounds = nullptr;
  IControl* mCaller = nullptr; // Pointer to the IControl that created this pop-up menu, for callback
  IPopupMenu* mMenu = nullptr; // Pointer to the main IPopupMenu, that this control is visualising. This control does not own the menu.

  int mMaxColumnItems = 0; // How long the list can get before adding a new column - 0 equals no limit
  bool mScrollIfTooBig = true; // If the menu is higher than the graphics context, should it scroll or should it start a new column //FIXME: not yet scrolling
  
  float mCellGap = 2.f; // The gap between cells in pixels
  float mSeparatorSize = 2.; // The size in pixels of a separator. This could be width or height
  const float TEXT_HPAD = 5.; // The amount of horizontal padding on either side of cell text in pixels
  const float TICK_SIZE = 10.; // The size of the area on the left of the cell where a tick mark appears on checked items - actual
  const float ARROW_SIZE = 8; // The width of the area on the right of the cell where an arrow appears for new submenus
  const float mPadding = 5.; // How much white space between the background and the cells
  float mRoundness = 5.f; // The roundness of the corners of the menu panel backgrounds
  float mDropShadowSize = 10.f; // The size in pixels of the drop shadow
  float mOpacity = 0.95f; // The opacity of the menu panel backgrounds when fully faded in

protected:
  IRECT mSpecifiedCollapsedBounds;
  IRECT mSpecifiedExpandedBounds;
};
