import { useState, useEffect } from 'react';
import { getFeaturedProducts, getCategories, getAllProducts } from '../api/api';
import { fallbackProducts, fallbackCategories } from '../data/fallbackProducts';
import ProductCard from '../components/ProductCard';
import './Home.css';

const Home = () => {
  const [featuredProducts, setFeaturedProducts] = useState([]);
  const [categories, setCategories] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  const [usingFallback, setUsingFallback] = useState(false);

  useEffect(() => {
    loadData();
  }, []);

  const loadData = async () => {
    try {
      setLoading(true);
      setError(null);
      setUsingFallback(false);
      setFeaturedProducts([]);
      setCategories([]);
      const [productsRes, categoriesRes] = await Promise.allSettled([
        getFeaturedProducts(),
        getCategories(),
      ]);

      let products = [];
      if (productsRes.status === 'fulfilled' && productsRes.value?.data?.success) {
        products = productsRes.value.data.data || [];
      }
      if (products.length === 0) {
        const fallbackRes = await getAllProducts().catch(() => null);
        if (fallbackRes?.data?.success && Array.isArray(fallbackRes.data.data)) {
          products = fallbackRes.data.data.slice(0, 8);
        }
      }
      if (products.length === 0) {
        products = fallbackProducts;
        setUsingFallback(true);
      }
      setFeaturedProducts(products);

      let cats = [];
      if (categoriesRes.status === 'fulfilled' && categoriesRes.value?.data?.success) {
        cats = categoriesRes.value.data.data || [];
      }
      if (cats.length === 0 && products.length > 0) {
        cats = fallbackCategories;
      }
      setCategories(cats);
    } catch (err) {
      console.error('Error loading home data:', err);
      setFeaturedProducts(fallbackProducts);
      setCategories(fallbackCategories);
      setUsingFallback(true);
    } finally {
      setLoading(false);
    }
  };

  if (loading) {
    return <div className="loading">Loading...</div>;
  }

  return (
    <div className="home">
      <section className="hero">
        <h1>Welcome to LALA STORE</h1>
        <p>Discover the latest fashion trends</p>
      </section>

      {categories.length > 0 && (
        <section className="categories-section">
          <h2>Shop by Category</h2>
          <div className="categories-grid">
            {categories.map((category) => (
              <div key={category.id} className="category-card">
                <h3>{category.name}</h3>
                <p>{category.description}</p>
              </div>
            ))}
          </div>
        </section>
      )}

      <section className="featured-section">
        <h2>Featured Products</h2>
        {error && featuredProducts.length === 0 ? (
          <p className="no-products">{error} <button type="button" className="retry-btn" onClick={loadData}>Retry</button></p>
        ) : featuredProducts.length === 0 ? (
          <p className="no-products">No products available at the moment. <button type="button" className="retry-btn" onClick={loadData}>Retry</button></p>
        ) : (
          <>
            {usingFallback && (
              <p className="fallback-notice">Showing demo products (backend offline). Start the backend for live data.</p>
            )}
            <div className="products-grid">
              {featuredProducts.map((product) => (
                <ProductCard key={product.id} product={product} />
              ))}
            </div>
          </>
        )}
      </section>
    </div>
  );
};

export default Home;
