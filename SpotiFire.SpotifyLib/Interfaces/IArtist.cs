﻿using System;
namespace SpotiFire.SpotifyLib
{
    public interface IArtist : ISpotifyObject, IAsyncLoaded
    {
        bool IsLoaded { get; }
        string Name { get; }
        IArtistBrowse Browse(ArtistBrowseType type = ArtistBrowseType.NoTracks);
    }
}
