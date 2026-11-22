import React from 'react';

const YouTubePlayer = ({ videoId, author, description, aspectRatio = '56.25%' }) => {
    return (
        <div style={{ margin: '20px 0' }}>
            {/* Video Container */}
            <div style={{ position: 'relative', paddingBottom: aspectRatio, height: 0 }}>
                <iframe
                    src={`https://www.youtube.com/embed/${videoId}`}
                    style={{
                        position: 'absolute',
                        top: 0,
                        left: 0,
                        width: '100%',
                        height: '100%',
                    }}
                    title='YouTube video player'
                    frameBorder="0"
                    allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture"
                    allowFullScreen
                />
            </div>

            {/* Context Section */}
            <div style={{ textAlign: 'center', marginTop: '10px', fontSize: '0.9rem' }}>
                {author && <p style={{ fontStyle: 'italic', margin: '5px 0' }}>By: {author}</p>}
                {description && <p style={{ margin: '5px 0', color: '#555' }}>{description}</p>}
            </div>
        </div>
    );
};

export default YouTubePlayer;