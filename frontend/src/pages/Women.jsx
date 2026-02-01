import { useState, useEffect } from 'react';
import { getProductsByGender } from '../api/api';
import ProductCard from '../components/ProductCard';
import './Women.css';

const Women = () => {
  const [products, setProducts] = useState([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    loadProducts();
  }, []);

  const loadProducts = async () => {
    try {
      setLoading(true);
      const response = await getProductsByGender('women');
      if (response.data.success) {
        setProducts(response.data.data);
      }
    } catch (error) {
      console.error('Error loading women products:', error);
    } finally {
      setLoading(false);
    }
  };

  if (loading) {
    return <div className="loading">Loading...</div>;
  }

  return (
    <div className="women-page">
      <div className="page-header">
        <h1>Women's Collection</h1>
        <p>Discover our latest women's fashion</p>
      </div>
      {products.length === 0 ? (
        <p className="no-products">No women's products available at the moment.</p>
      ) : (
        <div className="products-grid">
          {products.map((product) => (
            <ProductCard key={product.id} product={product} />
          ))}
        </div>
      )}
    </div>
  );
};

export default Women;
