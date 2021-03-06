template <typename TextureAdaptor>
FontTextureCache::FontTextureCache(TextureAdaptor textureAdaptor):
    mGlyphPacker(32),
    mCurrentSize(32)
{
    std::shared_ptr<TextureAdaptor> textureAdaptorToKeep = std::make_shared<TextureAdaptor>(textureAdaptor);
    textureAdaptorToKeep->initialize(32, 32);

    mResizeStorage = [=] (uint32_t width, uint32_t height)
    {
        textureAdaptorToKeep->resize(width, height);
    };

    mWriteBitmap = [=] (uint32_t x, uint32_t y, const BitMap& bitMap)
    {
        textureAdaptorToKeep->writeBitmap(x, y, bitMap);
    };

    //setup filled white area to use for things like underscore
    gim::BitMap whiteSquare;
    whiteSquare.width = 8;
    whiteSquare.height = 8;
    whiteSquare.pixels = std::vector<uint8_t>(8 * 8, 255);
    auto rectangle = mGlyphPacker.insert({8, 8});
    mSolidCoords = rectangle;
    mWriteBitmap(rectangle.start.x, rectangle.start.y, whiteSquare);
}
