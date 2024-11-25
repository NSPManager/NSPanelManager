import React from 'react';
import styles from './styles.module.css';

const CenteredImage = ({ src, alt, width = 'auto', height = 'auto', figureNumber }) => {
    return (
        <figure className={styles.centeredImageContainer}>
            <img src={src} alt={alt} style={{width, height}}/>
            {figureNumber && (
                <figcaption className={styles.caption}>
                    Figure {figureNumber}: {alt}
                </figcaption>
            )}
        </figure>
    );
};

export default CenteredImage;