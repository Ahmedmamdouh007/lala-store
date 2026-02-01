import { useState, useEffect } from 'react';
import { getProductsByGender } from '../api/api';
import ProductCard from '../components/ProductCard';
import './Men.css';

const Men = () => {
  const [products, setProducts] = useState([]);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    loadProducts();
  }, []);

  const loadProducts = async () => {
    try {
      setLoading(true);
      const response = await getProductsByGender('men');
      if (response.data.success) {
        setProducts(response.data.data);
      }
    } catch (error) {
      console.error('Error loading men products:', error);
    } finally {
      setLoading(false);
    }
  };

  if (loading) {
    return <div className="loading">Loading...</div>;
  }

  return (
    <div className="men-page">
      <div className="page-header">
        <h1>Men's Collection</h1>
        <p>Discover our latest men's fashion</p>
      </div>
      {products.length === 0 ? (
        <p className="no-products">No men's products available at the moment.</p>
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

export default Men;
